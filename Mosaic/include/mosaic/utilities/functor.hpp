#pragma once

/*! @file functor.hpp
 *  @brief Generate class inheritance hierarchies using typelists.
 */


#include <memory>
#include <utility>
#include "type_traits.hpp"


namespace mosaic {

    namespace fun_internal {

        /** @brief Abstract class defining interface for handlers for various callables.
         */
        template <typename RetT, typename... Params>
        class FunctorImpl {
        
        public:
            virtual RetT operator()(Params&&...) const = 0;
            std::unique_ptr<FunctorImpl> clone() const;
            virtual ~FunctorImpl() = default;
        
        private:
            // Separated implementation for `clone()` to workaround the 
            // covariant return type restrictions on smart pointers.
            virtual FunctorImpl* clone_impl() const = 0; 
        
        };


        /** @brief Handler for functors and functor like objects (eg. lambdas).
         */
        template <typename Fun, typename ParentRetT, typename... ParentParams>
        class FunctorHandler
            : public FunctorImpl<ParentRetT, ParentParams...>
        {

        public:   

            // Constructors
            FunctorHandler(const Fun& fun): fun_(fun) {}
            FunctorHandler(Fun&& fun): fun_(std::move(fun)) {}

            std::unique_ptr<FunctorHandler> clone() const {
                return clone_impl();
            }

            ParentRetT operator ()(ParentParams&&... params) const override {
                return fun_(std::forward<ParentParams>(params) ...);
            }

        private:

            FunctorHandler* clone_impl() const override {
                return new FunctorHandler(*this);
            }
            
            // Data members
            Fun fun_;

        };


        /** @brief Handler for fuctions.
         */
        template <typename FunctionT, typename ParentRetT, typename... ParentParams>
        class FunctionHandler
            : public FunctorImpl<ParentRetT, ParentParams...>
        {

        public:   

            // Constructor
            FunctionHandler(const FunctionT& fun): fun_(fun) {}

            std::unique_ptr<FunctionHandler> clone() const {
                return clone_impl();
            }

            ParentRetT operator ()(ParentParams&&... params) const override {
                return fun_(std::forward<ParentParams>(params)...);
            }

        private:

            FunctionHandler* clone_impl() const override {
                return new FunctionHandler(*this);
            }
            

            FunctionT fun_;

        };


        /** @brief Handler for pointer to member function.
         */
        template <
            typename PointerToObj, typename PointerToMemFun,
            typename ParentRetT, typename... ParentParams
        >
        class MemFunHandler
            : public FunctorImpl<ParentRetT, ParentParams...>
        {
        
        public:   

            // Constructor
            MemFunHandler(const PointerToObj& p_obj, PointerToMemFun p_mem_fun)
                : p_obj_(p_obj), p_mem_fun_(p_mem_fun) {}

            std::unique_ptr<MemFunHandler> clone() const {
                return clone_impl();
            }

            ParentRetT operator ()(ParentParams&&... params) const override {
                return (p_obj_->*p_mem_fun_)(std::forward<ParentParams>(params) ...);
            }

        private:

            MemFunHandler* clone_impl() const override {
                return new MemFunHandler(*this);
            }
            
            // `PointerToObj` kept as template parameter.
            // Allows user to decide which smart/raw pointer to use.
            PointerToObj p_obj_; 
            PointerToMemFun p_mem_fun_;
        };

    } // end `fun_internal` namespace


    /** @brief Functor template class to store callable entities.
     * 
     *  @details Implements the "Command" design pattern which encapsulates a request in an object.
     *  The design pattern,
     *  - Allows delayed execution of callable entities.
     *  - Isolates the command object invoker from the request/callable itself.
     * 
     * Works with different callable entities
     */
    template <typename ReturnT, typename... Params>
    class Functor {

    private:
        using FunImpl = fun_internal::FunctorImpl<ReturnT, Params...>;
        template <class Function> using FunctorHandler = fun_internal::FunctorHandler<Function, ReturnT, Params...>;
        template <class Fun> using FunctionHandler = fun_internal::FunctionHandler<Fun, ReturnT, Params...>;
        template <class ObjPointer, class MemFunPointer> using MemFunHandler = fun_internal::MemFunHandler<ObjPointer, MemFunPointer, ReturnT, Params...>;
    
    public:

        // Constructors
        Functor(): upImpl_() {}
        explicit Functor(std::unique_ptr<FunImpl> upImpl): upImpl_(std::move(upImpl_)) {}

        // Accept other functor objects
        template <class Fun>
        Functor(Fun&& fun): 
            upImpl_(
                std::make_unique<
                    FunctorHandler<typename TypeTraits<Fun>::ReferredType>
                >(std::forward<Fun>(fun))
            ) {}
        
        // Accept functions
        template <typename FRetT, typename... FParams>
        Functor(FRetT (&fun)(FParams ...)): 
            upImpl_(
                std::make_unique<
                    FunctionHandler<FRetT(&)(FParams...)>
                >(fun)
            ) {}

        // Accept a object pointer, pointer to member function pair
        template<class ObjPointer, class MemFunPointer>
        Functor(ObjPointer p_obj, MemFunPointer p_mem_fun ): 
            upImpl_(
                std::make_unique<
                    MemFunHandler<ObjPointer, MemFunPointer>
                >(p_obj, p_mem_fun)
            ) {}


        // Move constructor
        Functor(Functor&& other): upImpl_(std::move(other.upImpl_)) {}
        // Move assignment operator
        Functor& operator=(Functor&& rhs) {
            if (this != &rhs) {
                upImpl_ = std::move(rhs.upImpl_);
            }
            return *this;
        }

        // Forwarding call operator
        ReturnT operator()(Params&&... params) const {
            return (*upImpl_)(std::forward<Params>(params) ...);
        }

        // Clone member function
        Functor clone() const {
            return Functor(std::make_unique<FunImpl>(upImpl_->clone()));
        }

    private:
        std::unique_ptr<FunImpl> upImpl_;

    };

} // end namespace `mosaic`