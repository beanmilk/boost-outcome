/* Policies for result and outcome
(C) 2017-2018 Niall Douglas <http://www.nedproductions.biz/> (59 commits)
File Created: Oct 2017


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef BOOST_OUTCOME_POLICY_OUTCOME_EXCEPTION_PTR_RETHROW_HPP
#define BOOST_OUTCOME_POLICY_OUTCOME_EXCEPTION_PTR_RETHROW_HPP

#include "result_exception_ptr_rethrow.hpp"

BOOST_OUTCOME_V2_NAMESPACE_EXPORT_BEGIN

namespace policy
{
  /*! Policy interpreting `EC` or `E` as a type for which `trait::has_exception_ptr_v<EC|E>` is true.
  Any wide attempt to access the successful state where there is none causes:
  `std::rethrow_exception(policy::exception_ptr(.error()|.exception()))` appropriately.
  */
  template <class T, class EC, class E> struct exception_ptr_rethrow : detail::base
  {
    /*! Performs a wide check of state, used in the value() functions
    \effects If outcome does not have a value, if it has an exception it rethrows that exception via `std::rethrow_exception()`,
    if it has an error it rethrows that error via `std::rethrow_exception()`, else it throws `bad_outcome_access`.
    */
    template <class Impl> static constexpr void wide_value_check(Impl &&self)
    {
      if((self._state._status & BOOST_OUTCOME_V2_NAMESPACE::detail::status_have_value) == 0)
      {
        if((self._state._status & BOOST_OUTCOME_V2_NAMESPACE::detail::status_have_exception) != 0)
        {
          using Outcome = BOOST_OUTCOME_V2_NAMESPACE::detail::rebind_type<outcome<T, EC, E, exception_ptr_rethrow>, decltype(self)>;
          Outcome _self = static_cast<Outcome>(self);  // NOLINT
          detail::rethrow_exception<trait::has_exception_ptr_v<E>>{std::forward<Outcome>(_self)._ptr};
        }
        if((self._state._status & BOOST_OUTCOME_V2_NAMESPACE::detail::status_have_error) != 0)
        {
          detail::rethrow_exception<trait::has_exception_ptr_v<EC>>{std::forward<Impl>(self)._error};
        }
        BOOST_OUTCOME_THROW_EXCEPTION(bad_outcome_access("no value"));
      }
    }
    /*! Performs a wide check of state, used in the error() functions
    \effects If outcome does not have an error, it throws `bad_outcome_access`.
    */
    template <class Impl> static constexpr void wide_error_check(Impl &&self)
    {
      if((self._state._status & BOOST_OUTCOME_V2_NAMESPACE::detail::status_have_error) == 0)
      {
        BOOST_OUTCOME_THROW_EXCEPTION(bad_outcome_access("no error"));
      }
    }
    /*! Performs a wide check of state, used in the exception() functions
    \effects If result does not have an exception, it throws `bad_outcome_access`.
    */
    template <class Impl> static constexpr void wide_exception_check(Impl &&self)
    {
      if((self._state._status & BOOST_OUTCOME_V2_NAMESPACE::detail::status_have_exception) == 0)
      {
        BOOST_OUTCOME_THROW_EXCEPTION(bad_outcome_access("no exception"));
      }
    }
  };
}  // namespace policy

BOOST_OUTCOME_V2_NAMESPACE_END

#endif
