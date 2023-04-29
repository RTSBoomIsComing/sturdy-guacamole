#pragma once
#include <d3d11.h>
#include <comdef.h>


namespace sturdy_guacamole
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			_com_error err(hr);
			LPCTSTR errMsg = err.ErrorMessage();
			::OutputDebugString(errMsg);

			throw hr;
		}
	}
}