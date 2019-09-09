#include "ExOverlappedIO.h"
#include "Session.h"

ExOverlappedIO::ExOverlappedIO(Session* owner, IOType ioType)
	: mSessionObject(owner), mIoType(ioType)
{
	memset(&mOverlapped, 0, sizeof(OVERLAPPED));
	memset(&mWsaBuf, 0, sizeof(WSABUF));

	mSessionObject->AddRef();
}



void DeleteIoContext(ExOverlappedIO* context)
{
	if (nullptr == context)
	{
		return;
	}

	context->mSessionObject->ReleaseRef();
	delete static_cast<ExOverlappedIO*>(context);
}

