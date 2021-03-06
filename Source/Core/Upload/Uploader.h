/*

    Image Uploader -  free application for uploading images/files to the Internet

    Copyright 2007-2015 Sergey Svistunov (zenden2k@gmail.com)

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/

#ifndef _UPLOADER_H_
#define _UPLOADER_H_

#include <string>
#include "Core/Utils/CoreTypes.h"
#include "Core/Network/NetworkClient.h"
#include "Core/Upload/UploadEngine.h"
#include "Core/3rdpart/FastDelegate.h"

class CUploader
{
	public:
		CUploader(void);
		~CUploader(void);
		
		bool setUploadEngine(CAbstractUploadEngine* UploadEngine);
		CAbstractUploadEngine * getUploadEngine();
		
		void setThumbnailWidth(int width);
		bool UploadFile(const std::string & FileName, const std::string displayFileName);
		bool Upload(UploadTask* task);
		const std::string  getDownloadUrl();
		const std::string  getDirectUrl();
		const std::string  getThumbUrl();
		void stop();
		bool needStop();
		// events
		fastdelegate::FastDelegate0<bool> onNeedStop;
		fastdelegate::FastDelegate2<CUploader*,InfoProgress> onProgress;
		fastdelegate::FastDelegate3<StatusType, int, std::string> onStatusChanged;
		fastdelegate::FastDelegate2<const std::string&, bool> onDebugMessage;
		fastdelegate::FastDelegate1<ErrorInfo> onErrorMessage;
		fastdelegate::FastDelegate1<NetworkClient*> onConfigureNetworkClient;

		void DebugMessage(const std::string& message, bool isServerResponseBody = false);
		void SetStatus(StatusType status, int param1=0, std::string param="");
		StatusType GetStatus() const;
	protected:
		InfoProgress m_PrInfo;
		static int pluginProgressFunc (void* userData, double dltotal,double dlnow,double ultotal, double ulnow);

		bool m_bShouldStop;
		int m_nThumbWidth;
		StatusType m_CurrentStatus;
		std::string m_FileName;
		std::string m_displayFileName;
		
		std::string m_ThumbUrl;
		std::string m_ImageUrl;
		std::string m_DownloadUrl;
		std::string m_ErrorReason;
		
		void Error(bool error, std::string message, ErrorType type = etOther, int retryIndex = -1);
		void ErrorMessage(ErrorInfo);
		NetworkClient m_NetworkClient;
		CAbstractUploadEngine *m_CurrentEngine;
		void Cleanup();
	private:
		DISALLOW_COPY_AND_ASSIGN(CUploader);
};

#endif
