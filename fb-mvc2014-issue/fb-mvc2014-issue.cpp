#include <functional>
#include "Deferred.h"

int main()
{

	auto good = std::function<FB::Promise<std::string>(void)>([]() -> FB::Promise<std::string> {
		FB::Promise<std::string> promise = "5";
		return promise;
	});

	auto func = std::function<FB::Promise<FB::variant>(void)>([]() -> FB::Promise<FB::variant> {
		FB::Promise<FB::variant> promise = "5";
		return promise;
	});


    return 0;
}

