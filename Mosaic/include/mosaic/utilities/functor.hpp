#pragma once

/*! @file functor.hpp
 *  @brief Provides `Functor` class and related utilities.
 */


#include <memory>
#include <utility>
#include "type_traits.hpp"
#if __cplusplus >= 202002L // Compiler supports C++20 standard and above.
    #include <concepts>
#endif

namespace mosaic {

    namespace fun_internal {

        /** @brief Abstract class defining interface for handlers for various callables.
         */
        template <typename RetT, typename... Params>
        class FunctorImpl {
        
        public:
            virtual RetT operator()(Params...) const = 0;
            virtual ~FunctorImpl() = default;
        
            std::unique_ptr<FunctorImpl> clone() const {
                return std::unique_ptr<FunctorImpl>(this->clone_impl());
            }

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

            // Copy/Move constructors
            FunctorHandler(const Fun& fun): fun_(fun) {}
            FunctorHandler(Fun&& fun) noexcept: fun_(std::move(fun)) {}


            std::unique_ptr<FunctorHandler> clone() const {
                return clone_impl();
            }

            ParentRetT operator ()(ParentParams... params) const override {
                return fun_(static_cast<ParentParams>(params) ...);
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

            // Copy constructor
            FunctionHandler(const FunctionT& fun): fun_(fun) {}


            std::unique_ptr<FunctionHandler> clone() const {
                return clone_impl();
            }

            ParentRetT operator ()(ParentParams... params) const override {
                return fun_(static_cast<ParentParams>(params)...);
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

            ParentRetT operator ()(ParentParams... params) const override {
                return (p_obj_->*p_mem_fun_)(static_cast<ParentParams>(params) ...);
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
        // ------------
        
        Functor() = default;

        explicit Functor(std::unique_ptr<FunImpl> upImpl): upImpl_(std::move(upImpl)) {}

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
        Functor(FRetT (&fun)(FParams ...))
        : upImpl_(
                std::make_unique<
                    FunctionHandler<FRetT(&)(FParams...)>
                >(fun)
            ) 
        {
            // Better error message if arguments/return types are not convertible. 
            static_assert((Conversion<FParams, Params>::exists && ...), "One or more parameter types are not convertible!");
            static_assert(Conversion<FRetT, ReturnT>::exists, "Return type is not convertible!"); 
        }

        // Accept a object pointer, pointer to member function pair
        template<class ObjPointer, class MemFunPointer>
        Functor(ObjPointer p_obj, MemFunPointer p_mem_fun ): 
            upImpl_(
                std::make_unique<
                    MemFunHandler<ObjPointer, MemFunPointer>
                >(p_obj, p_mem_fun)
            ) {}
        
        // ------------

        // Copy/Move constructors/assignment operators
        // -------------------------------------------

        // (Deep) Copy constructor
        Functor(const Functor& other): upImpl_(std::move(other.clone().upImpl_)) {}
        // Move constructor
        Functor(Functor&& other) noexcept: upImpl_(std::move(other.upImpl_)) {}
        
        // (Deep) Copy assignment operator
        Functor& operator=(Functor rhs) noexcept{
            swap(*this, rhs);
            return *this;
        }
        // Move assignment operator
        Functor& operator=(Functor&& rhs) noexcept{
            swap(*this, rhs);
            return *this;
        }

        // -------------------------------------------

        friend void swap(Functor& lhs, Functor& rhs) noexcept {
            using std::swap;
            swap(lhs.upImpl_, rhs.upImpl_);
        }

        // Forwarding call operator
        ReturnT operator()(Params... params) const {
            return (*upImpl_)(static_cast<Params>(params) ...);
        }

        // Clone member function
        Functor clone() const {
            std::unique_ptr<FunImpl> u_ptr = upImpl_->clone();
            return Functor(std::move(u_ptr));
        }

    private:

        std::unique_ptr<FunImpl> upImpl_;

    };

    /**************************************************/

    namespace fun_internal {


        /** @brief Class that stores and binds the first argument for a `Fuctor`.
         */
        template <typename FunctorT>
        class BinderFirst;

        template <typename RetT, typename BoundParamT, typename... UnboundParamsT>
        class BinderFirst <Functor<RetT, BoundParamT, UnboundParamsT...>>
            : public fun_internal::FunctorImpl<RetT, UnboundParamsT...>
        {

        private:

            using InFunctor = Functor<RetT, BoundParamT, UnboundParamsT...>;
            using OutFunctor = Functor<RetT, UnboundParamsT...>;
            using BoundParamType = BoundParamT;

        public:

            // Constructors
            BinderFirst(const InFunctor& func, BoundParamT bound): func_(func), bound_(bound) {};
            BinderFirst(const InFunctor& func, BoundParamT&& bound): func_(func), bound_(std::move(bound)) {};
            BinderFirst(InFunctor&& func, BoundParamT bound): func_(std::move(func)), bound_(bound) {};
            BinderFirst(InFunctor&& func, BoundParamT&& bound) noexcept: func_(std::move(func)), bound_(std::move(bound)) {};


            RetT operator()(UnboundParamsT... unbound_params) const override {
                return func_(bound_, static_cast<UnboundParamsT>(unbound_params) ...);
            }

            std::unique_ptr<BinderFirst> clone() const {
                return clone_impl();
            }

        private:

            BinderFirst* clone_impl() const override {
                return new BinderFirst(*this);
            }

            BoundParamT bound_;
            InFunctor func_;

        };

    } // end `fun_internal` namespace


    /** @brief Helper function to bind first argument of a `Functor`
     *  @sa Functor
     */
    template <typename RetT, typename BoundParamT, typename... UnboundParamsT>
    Functor<RetT, UnboundParamsT...>
    BindFirst(
        const Functor<RetT, BoundParamT, UnboundParamsT...>& fun,
        BoundParamT bound) 
    {

        using InFunctor = Functor<RetT, BoundParamT, UnboundParamsT...>;
        using OutFunctor = Functor<RetT, UnboundParamsT...>;
        using OutFuncImpl = fun_internal::FunctorImpl<RetT, UnboundParamsT...>;
     
        std::unique_ptr<OutFuncImpl> u_ptr = 
            std::make_unique<fun_internal::BinderFirst<InFunctor>>(fun, bound);
     
        return OutFunctor(std::move(u_ptr));

    }

    /**************************************************/

} // end namespace `mosaic`