#ifndef NEOVM_EXCEPTIONS_HPP
#define NEOVM_EXCEPTIONS_HPP

#include <neovm/config.hpp>

#include <exception>
#include <stdint.h>
#include <string>
#include <memory>

namespace neo
{
	namespace vm
	{

		enum ErrorCode
		{
			OK = 0,
			SIMPLE_ERROR = 1,
			MEMORY_ERROR = 2,
			OVER_GAS_LIMIT = 3,

			UNKNOWN_ERROR = 100
		};

		class NeoVmException : public std::exception
		{
		protected:
			ErrorCode _code;
			std::string _name_value;
			std::string _error_msg;
		public:
			inline NeoVmException() : _code(ErrorCode::SIMPLE_ERROR) { }
			inline NeoVmException(ErrorCode code, const std::string &name_value, const std::string &error_msg)
				: _code(code), _name_value(name_value), _error_msg(error_msg) {}
			inline NeoVmException(const NeoVmException& other) {
				_code = other._code;
				_name_value = other._name_value;
				_error_msg = other._error_msg;
			}
			inline NeoVmException(const char *msg, ErrorCode code=ErrorCode::SIMPLE_ERROR)
			{
				_code = code;
				_error_msg = msg;
			}
			inline NeoVmException& operator=(const NeoVmException& other) {
				if (this != &other)
				{
					_error_msg = other._error_msg;
				}
				return *this;
			}
			inline virtual ~NeoVmException() {}

#ifdef WIN32
			inline virtual const char* what() const
#else
			inline virtual const char* what() const noexcept
#endif 
			{
				return _error_msg.c_str();
			}
			inline virtual ErrorCode code() const {
				return _code;
			}
			inline std::string name() const
			{
				return _name_value;
			}
			inline virtual std::shared_ptr<neo::vm::NeoVmException> dynamic_copy_exception()const
			{
				return std::make_shared<NeoVmException>(*this);
			}
			inline virtual void dynamic_rethrow_exception()const
			{
				if (code() == 0)
					throw *this;
				else
					neo::vm::NeoVmException::dynamic_rethrow_exception();
			}
		};
	}
}

#endif
