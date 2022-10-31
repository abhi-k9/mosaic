#pragma once

/*! @file singleton.hpp
 *  @brief Provides utilities related to creating and managing a singleton.
 */


#include <cstdlib>
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <atomic>

namespace mosaic {

    class Singleton {

    public:

        static Singleton& Instance() {
            if (!pInstance_) {
                if (is_destroyed_) {
                    on_dead_reference();
                }
                else {
                    create();
                }
            }

            return *pInstance_;
        }

    private:

        Singleton();
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;
        Singleton& operator=(Singleton&&) = delete;
        virtual ~Singleton() {
            pInstance_ = nullptr;
            is_destroyed_ = true;
        }


        static void create() {
            static Singleton s_instance;
            pInstance_ = &s_instance;
        }

        static void on_dead_reference() {
            throw std::runtime_error("Dead reference detected!");
        }

        // C++17 `static inline` variable.
        // Allows allocating space for and initializing the variable by declaring inside the class.
        static inline Singleton *pInstance_ = nullptr;
        static inline bool is_destroyed_ = false;

    };

    namespace lifetime_impl {

        class LifetimeTracker {
        public:
            LifetimeTracker(unsigned int x): longevity_(x) {}
            
            virtual ~LifetimeTracker() = 0;
            
            friend inline bool Compare(
                unsigned int longevity,
                const LifetimeTracker * p
            );

        private:
            unsigned int longevity_;

        };

        inline LifetimeTracker::~LifetimeTracker() {}
                    
        inline bool Compare(
            unsigned int longevity,
            const LifetimeTracker * p
        ) {
            return p -> longevity_ > longevity;
        }

        typedef LifetimeTracker** TrackerArray;
        extern TrackerArray pTrackerArray;
        extern unsigned int elements;

        template <typename T>
        struct Deleter {
            static void Delete(T* pObj) {
                delete pObj;
            }
        };

        template <typename T, typename Destroyer>
        class ConcreteLifeTimeTracker : public LifetimeTracker {
        public:
            ConcreteLifeTimeTracker(
                T* p,
                unsigned int longevity,
                Destroyer d)
                : LifetimeTracker(longevity),
                  pTracked_(p),
                  destroyer_(d)
            {}

            ~ConcreteLifeTimeTracker() noexcept {
                destroyer_(pTracked_);
            }

        private:
            T* pTracked_;
            Destroyer destroyer_;
        };

        static void AtExitFn() {
            assert(elements > 0 && pTrackerArray != nullptr);
            LifetimeTracker* pTop = pTrackerArray[elements-1];
            
            if (--elements == 0) {   // Decrement number of elements
                std::free(pTrackerArray);
                pTrackerArray = nullptr;
            } else {
                pTrackerArray = static_cast<TrackerArray>(std::realloc(pTrackerArray, elements*sizeof(LifetimeTracker*)));
            }
            delete pTop;
        }

    } // end `lifetime_imp` namespace

    /*!
     *  @note Do not apply to objects whose lifetimes are controlled by the compiler.
     *  Eg. regular global objects, static objects, and automatic objects.
     */
    template <typename T, typename Destroyer>
    void SetLongevity(T* pDynObject, unsigned int longevity,
                      Destroyer d = lifetime_impl::Deleter<T>::Delete) {

        lifetime_impl::TrackerArray pNewArray = static_cast<lifetime_impl::TrackerArray> (
            std::realloc(lifetime_impl::pTrackerArray, (lifetime_impl::elements + 1)*sizeof(lifetime_impl::LifetimeTracker*))
        );

        // Check for `realloc` failure
        if (!pNewArray) {
            throw std::bad_alloc();
        }
    
        // Create new `LifetimeTracker` object
        lifetime_impl::LifetimeTracker* p = new lifetime_impl::ConcreteLifeTimeTracker<T, Destroyer>(
            pDynObject, longevity, d
        );

        // Make space for new `LifeTimeTracker`
        lifetime_impl::TrackerArray pos = std::upper_bound(
            lifetime_impl::pTrackerArray, lifetime_impl::pTrackerArray + lifetime_impl::elements, longevity, lifetime_impl::Compare
        );
        std::copy_backward(pos, lifetime_impl::pTrackerArray + lifetime_impl::elements,
            lifetime_impl::pTrackerArray + lifetime_impl::elements + 1
        );

        // Insert the new `LifeTimeTracker`
        *pos = p;
        ++lifetime_impl::elements;
        std::atexit(lifetime_impl::AtExitFn);
    }

    namespace policies {

        // template <class T>
        // class CreaterPolicy {
        // public:
        //     static T* create();
        //     static void destroy(T* pobj);
        // };

        // template <class T>
        // class LifetimePolicy {
        // public:
        //     using PDestructionFunction = void (*)();
        //     static void schedule_destruction(PDestructionFunction);
        //     static void on_dead_reference();
        // };

        // // TODO
        // template <class T>
        // class ThreadingPolicy {
        //     using VolatileType;

        // };

        template <class T>
        class CreateUsingNew {
        public:
            static T* create() {
                return new T();
            }
            static void destroy(T* pobj) {
                delete pobj;
            }

        };

        template <class T>
        class DefaultLifetime {
        public:

            using PDestructionFunction = void (*)();

            static void schedule_destruction(PDestructionFunction p_destruction_function){
                std::atexit(p_destruction_function);
            }
            static void on_dead_reference(){
                throw std::runtime_error("Reuse of dead singleton detected!");
            }

        };        

        template <class T>
        class SingleThread {
        public:
            using VolatileType = T;
            using Lock = int;
        };

    } // end `policies` namespace

    template <
        class T,
        template <class> class CreationPolicy = policies::CreateUsingNew,
        template <class> class LifetimePolicy = policies::DefaultLifetime,
        template <class> class ThreadingModel = policies::SingleThread
    >
    class SingletonHolder {

        using InstanceType = typename ThreadingModel<T>::VolatileType;

    public:
        // static T& instance() {

        //     if (!p_instance_) {

        //        [[maybe_unused]] typename ThreadingModel<T>::Lock guard;
                
        //         if(!p_instance_) {

        //             if (destroyed_) {
        //                 LifetimePolicy<T>::on_dead_reference();
        //                 destroyed_ = false;
        //             }

        //             p_instance_ = CreationPolicy<T>::create();
        //             LifetimePolicy<T>::schedule_destruction(&destroy_singleton);
                    
        //         }

        //     }

        //     return *p_instance_;

        // };

        static T& instance() {

            Singleton* temp = p_instance_.load(std::memory_order_relaxed);
            std::atomic_thread_fence(std::memory_order_acquire);
            if (!temp) {

                [[maybe_unused]] typename ThreadingModel<T>::Lock guard;
                temp = p_instance_.load(std::memory_order_relaxed);
                if(!temp) {

                    if (destroyed_) {
                        LifetimePolicy<T>::on_dead_reference();
                        destroyed_ = false;
                    }

                    temp = CreationPolicy<T>::create();
                    std::atomic_thread_fence(std::memory_order_release);
                    p_instance_.store(temp, std::memory_order_relaxed);
                    
                    LifetimePolicy<T>::schedule_destruction(&destroy_singleton);
                    
                }

            }

            return *temp;

        };

    private:
        static void destroy_singleton() {
            assert(!destroyed_);
            CreationPolicy<T>::destroy(pInstance_);
            pInstance_ = 0;
            destroyed_ = true;
        };
        SingletonHolder() = delete;

        // static inline InstanceType* p_instance_ = nullptr;
        static inline std::atomic<InstanceType*> p_instance_ = nullptr;
        
        static inline bool destroyed_ = false;


    };

} // end `mosaic` namespace