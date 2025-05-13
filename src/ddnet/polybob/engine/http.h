#pragma once

#include <memory>

namespace polybob {

class IHttpRequest
{
};

class IHttp
{
public:
	virtual void Run(std::shared_ptr<IHttpRequest> pRequest) = 0;

	// IInterface
	virtual void Shutdown() {}
	virtual ~IHttp() {}
};

} // namespace polybob
