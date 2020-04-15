#ifndef CUSTOMEXCEPTION_H
#define CUSTOMEXCEPTION_H

#include <Windows.h>
#include <exception>
#include <sstream>
#include <string>
#include <vector>

class customException : public std::exception
{
public:
	customException(int line, const char* file, const char* type, const char* message) noexcept {
		std::ostringstream oss;
		oss << "[" << type << "]" << '\n'
			<< "[FILE] " << file << '\n'
			<< "[LINE] " << line << '\n'
			<< "[INFO]" << '\n' << message;
		info = std::move(oss.str());
	}

	customException(int line, const char* file, const char* type, HRESULT hr) noexcept {
		char* message = nullptr;
		const DWORD nMsgLen = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPSTR>(&message), 0, nullptr
		);
		if (nMsgLen == 0)
			message = const_cast<char*>("Unidentified error code");

		std::ostringstream oss;
		oss << "[" << type << "]" << '\n'
			<< "[FILE] " << file << '\n'
			<< "[LINE] " << line << '\n'
			<< "[INFO]" << '\n' << message;

		info = std::move(oss.str());
		if (nMsgLen > 0)
			LocalFree(message);
	}

#ifdef _DEBUG
	customException(int line, const char* file, const char* type, HRESULT hr, std::vector<std::string> infoMgr) noexcept {
		char* message = nullptr;
		const DWORD nMsgLen = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPSTR>(&message), 0, nullptr
		);
		if (nMsgLen == 0)
			message = const_cast<char*>("Unidentified error code");

		std::ostringstream oss;
		oss << "[" << type << "]" << '\n'
			<< "[FILE] " << file << '\n'
			<< "[LINE] " << line << '\n'
			<< "[INFO]" << '\n' << message <<
			"\n[DXGI INFO]";

		for (auto& str : infoMgr) {
			oss << '\n' << str << '\n';
		}

		info = std::move(oss.str());
		if (nMsgLen > 0)
			LocalFree(message);
	}
#endif

	const char* what() const noexcept override {
		return info.c_str();
	}

protected:
	mutable std::string info;

};

#define FAILED_LAST_EXCEPT(type, b) if(!b) throw customException(__LINE__, __FILE__, type, GetLastError())

#endif