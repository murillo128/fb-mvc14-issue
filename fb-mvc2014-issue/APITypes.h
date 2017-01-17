/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Dual license model; choose one of two:
New BSD License
http://www.opensource.org/licenses/bsd-license.php
- or -
GNU Lesser General Public License, version 2.1
http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_APITYPES
#define H_FB_APITYPES

#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <memory>
#include <cstdint>


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @namespace  FB
///
/// @brief  Primary location of FireBreath classes and utility functions.
///         
/// The five most important classes to understand when implementing a FireBreath plugin are:
///   - FB::PluginCore
///   - FB::JSAPI / FB::JSAPIAuto
///   - FB::BrowserHost
///   - FB::JSObject
///   - FB::variant
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace FB
{
	class variant;
	template <typename T>
	class Deferred;
	template <typename T>
	class Promise;

	using variantDeferred = Deferred < variant >;
	using variantPromise = Promise < variant >;
	namespace variant_detail {
		// Note that empty translates into return VOID (undefined)
		struct empty;
	}

	// Variant list

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @typedef    FB::VariantList
	///
	/// @brief  Defines an alias representing a vector of variants.
	/// @see FB::make_variant_list()
	/// @see FB::convert_variant_list()
	////////////////////////////////////////////////////////////////////////////////////////////////////
	using VariantList = std::vector < variant >;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @typedef    FB::VariantPromiseList
	///
	/// @brief  Defines an alias representing a vector of variantPromise objects.
	/// @see FB::make_variant_list()
	/// @see FB::convert_variant_list()
	////////////////////////////////////////////////////////////////////////////////////////////////////
	using VariantPromiseList = std::vector < variantPromise >;
	using VariantListPromise = Promise < VariantList >;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @typedef    FB::VariantMap
	///
	/// @brief  Defines an alias representing a string -> variant map.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	using VariantMap = std::map < std::string, variant >;
	using VariantMapPromise = Promise < VariantMap >;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @typedef    FB::StringSet
	///
	/// @brief  Defines an alias representing a set of std::strings
	////////////////////////////////////////////////////////////////////////////////////////////////////
	using StringSet = std::set < std::string >;

	


	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @struct CatchAll
	///
	/// @brief  When used as a parameter on a JSAPIAuto function this matches 0 or more variants
	///         -- in other words, all other parameters from this point on regardless of type.
	/// 
	/// This helper struct allows your scriptable methods to receive 0 or more parameters in addition
	/// to some fixed ones. E.g. given the following scriptable method:
	/// @code
	/// long howManyParams(long a, std::string b, const FB::CatchAll& more) {
	///     const FB::VariantList& values = more.value;
	///     long paramCount = 2 + values.size();
	///     return paramCount;
	/// }
	/// @endcode
	/// The following calls would result in:
	/// @code
	/// > obj.howManyParams(42, "moo");
	/// => returns 2
	/// > obj.howManyParams(42, "moo", 1.0, "meh");
	/// => returns 4
	/// @endcode
	///
	/// @author Georg Fritzsche
	/// @date   10/15/2010
	////////////////////////////////////////////////////////////////////////////////////////////////////
	struct CatchAll {
		typedef FB::VariantList value_type;
		FB::VariantList value;
	};

	// Special Variant types
	using FBVoid = FB::variant_detail::empty;
	struct FBNull {};
	struct FBDateString {
	public:
		FBDateString() { }
		FBDateString(const FBDateString& rhs) : date(rhs.date) { }
		FBDateString(const std::string &dstr) : date(dstr) { }

		FBDateString& operator=(std::string dstr) { date = dstr; return *this; }
		std::string getValue() { return date; }
		void setValue(std::string value) { date = value; }
		bool operator<(std::string rh) const
		{
			return date < rh;
		}
		bool operator<(const FBDateString& rh) const
		{
			return date < rh.date;
		}

	protected:
		std::string date;
	};

	// JSAPI methods

	class JSAPI;
	/// @brief  Defines an alias representing a function ptr for a method on a FB::JSAPISimple object
	using CallMethodPtr = variantPromise(JSAPI::*)(const std::vector<variant>&);
	/// @brief Used by FB::JSAPISimple to store information about a method
	struct MethodInfo {
		MethodInfo() : callFunc(nullptr) { }
		MethodInfo(CallMethodPtr callFunc) : callFunc(callFunc) { }
		MethodInfo(const MethodInfo &rh) : callFunc(rh.callFunc) { }
		CallMethodPtr callFunc;
	};

	/// @brief  Defines an alias representing a map of methods used by FB::JSAPISimple
	using MethodMap = std::map<std::string, MethodInfo>;

	// JSAPI properties


	/// @brief  Defines an alias representing a function pointer for a property getter on a FB::JSAPISimple object
	using GetPropPtr = variantPromise(JSAPI::*)();
	/// @brief  Defines an alias representing a function pointer for a property setter on a FB::JSAPISimple object
	using SetPropPtr = void (JSAPI::*)(const variant& value);
	/// @brief Used by FB::JSAPISimple to store information about a property
	struct PropertyInfo {
		PropertyInfo() : getFunc(nullptr), setFunc(nullptr) { }
		PropertyInfo(GetPropPtr getFunc, SetPropPtr setFunc) : getFunc(getFunc), setFunc(setFunc) { }
		PropertyInfo(const PropertyInfo &rh) : getFunc(rh.getFunc), setFunc(rh.setFunc) { }
		GetPropPtr getFunc;
		SetPropPtr setFunc;
	};

	/// @brief  Defines an alias representing a map of properties used by FB::JSAPISimple
	using PropertyMap = std::map < std::string, PropertyInfo >;

	// new style JSAPI methods
	/// @brief  Used to set a SecurityZone for a method or property -- used by JSAPIAuto
	using SecurityZone = int;

	/// @brief  Default SecurityZone values; you can use these or provide your own
	enum SecurityLevel {
		SecurityScope_Public = 0,
		SecurityScope_Protected = 2,
		SecurityScope_Private = 4,
		SecurityScope_Local = 6
	};

	/// @brief  Defines an alias representing a method functor used by FB::JSAPIAuto, created by FB::make_method().
	using CallMethodFunctor = std::function < variantPromise(const std::vector<variant>&) >;
	struct MethodFunctors
	{
		FB::CallMethodFunctor call;
		SecurityZone zone;
		MethodFunctors() : call() {}
		MethodFunctors(const CallMethodFunctor& call) : call(call) {}
		MethodFunctors(const SecurityZone& zone, const CallMethodFunctor& call) : call(call), zone(zone) {}
		MethodFunctors(const MethodFunctors& m) : call(m.call) {}
		MethodFunctors& operator=(const MethodFunctors& rhs) {
			call = rhs.call;
			zone = rhs.zone;
			return *this;
		}
	};
	/// @brief  Defines an alias representing a map of method functors used by FB::JSAPIAuto
	using MethodFunctorMap = std::map < std::string, MethodFunctors >;

	// new style JSAPI properties

	/// @brief  Defines an alias representing a property getter functor used by FB::JSAPIAuto
	using GetPropFunctor = std::function < variantPromise() >;
	/// @brief  Defines an alias representing a property setter functor used by FB::JSAPIAuto
	using SetPropFunctor = std::function < void(const FB::variant&) >;
	/// @brief  used by FB::JSAPIAuto to store property implementation details, created by FB::make_property().
	struct PropertyFunctors
	{
		GetPropFunctor get;
		SetPropFunctor set;
		PropertyFunctors() : get(), set() {}
		PropertyFunctors(const GetPropFunctor& get, const SetPropFunctor& set)
			: get(get), set(set) {}
		PropertyFunctors(const PropertyFunctors& p)
			: get(p.get), set(p.set) {}
		PropertyFunctors& operator=(const PropertyFunctors& rhs) {
			get = rhs.get;
			set = rhs.set;
			return *this;
		}
	};
	/// @brief  Defines an alias representing a map of property functors used by FB::JSAPIAuto
	using PropertyFunctorsMap = std::map < std::string, PropertyFunctors >;

	

	// implementation details

	struct Rect {
		int32_t top;
		int32_t left;
		int32_t bottom;
		int32_t right;
	};
}

// This needs to be included after all our classes are defined because it relies on types defined in this file
// TODO: can this be done better?
#include "variant.h"

#endif

