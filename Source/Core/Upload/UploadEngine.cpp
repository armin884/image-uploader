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

#include "UploadEngine.h"
#include <stdlib.h>
#include "Core/Utils/StringUtils.h"

CUploadEngineData::CUploadEngineData()
{
}

CUploadEngineList_Base::CUploadEngineList_Base()
{
}

CUploadEngineData* CUploadEngineList_Base::byIndex(size_t index) {
	if ( index < m_list.size() ) {
		return &m_list[index];
	} else {
		return NULL;
	}
}

int CUploadEngineList_Base::count()
{
	return m_list.size();
}

CUploadEngineData* CUploadEngineList_Base::byName(const std::string& name)
{
	for (size_t i = 0; i < m_list.size(); i++)
	{
		if (!IuStringUtils::stricmp(m_list[i].Name.c_str(), name.c_str()))
			return &m_list[i];
	}
	return 0;
}

CUploadEngineData*  CUploadEngineList_Base::firstEngineOfType(CUploadEngineData::ServerType type) {
	for (size_t i = 0; i < m_list.size(); i++)
	{
		if ( m_list[i].Type == type) {
			return &m_list[i];
		}
	}
	return 0;
}

int CUploadEngineList_Base::getRandomImageServer()
{
	std::vector<int> m_suitableServers;
	for (size_t i = 0; i < m_list.size(); i++)
	{
		if (m_list[i].NeedAuthorization != 2 && m_list[i].ImageHost)
			m_suitableServers.push_back(i);
	}
	return m_suitableServers[rand() % (m_suitableServers.size())];
}

int CUploadEngineList_Base::getRandomFileServer()
{
	std::vector<int> m_suitableServers;
	for (size_t i = 0; i < m_list.size(); i++)
	{
		if (m_list[i].NeedAuthorization != 2 && !m_list[i].ImageHost)
			m_suitableServers.push_back(i);
	}
	return m_suitableServers[rand() % m_suitableServers.size()];
}

int CUploadEngineList_Base::GetUploadEngineIndex(const std::string Name)
{
	for (size_t i = 0; i < m_list.size(); i++)
	{
		if (m_list[i].Name == Name)
			return i;
	}
	return -1;
}

/* CAbstractUploadEngine */

CAbstractUploadEngine::~CAbstractUploadEngine()
{
}

bool CAbstractUploadEngine::DebugMessage(const std::string& message, bool isServerResponseBody)
{
	if (onDebugMessage)
		onDebugMessage(message, isServerResponseBody);
	return true;
}

bool CAbstractUploadEngine::ErrorMessage(ErrorInfo ei)
{
	if (onErrorMessage)
		onErrorMessage(ei);
	return true;
}

void CAbstractUploadEngine::setServerSettings(ServerSettingsStruct settings)
{
	m_ServersSettings = settings;
}

ServerSettingsStruct CAbstractUploadEngine::serverSettings()
{
	return m_ServersSettings;
}

bool CAbstractUploadEngine::needStop()
{
	if (m_bShouldStop)
		return m_bShouldStop;
	if (onNeedStop)
		m_bShouldStop = onNeedStop();  // delegate call
	return m_bShouldStop;
}

void CAbstractUploadEngine::SetStatus(StatusType status, std::string param)
{
	if (onStatusChanged)
		onStatusChanged(status, 0,  param);
}

void CAbstractUploadEngine::setNetworkClient(NetworkClient* nm)
{
	m_NetworkClient = nm;
}

void CAbstractUploadEngine::setUploadData(CUploadEngineData* data)
{
	m_UploadData = data;
}

CAbstractUploadEngine::CAbstractUploadEngine()
{
	m_bShouldStop = 0;
	m_NetworkClient = 0;
	m_UploadData = 0;
}

void CAbstractUploadEngine::setThumbnailWidth(int width)
{
	m_ThumbnailWidth = width;
}

CUploadEngineData* CAbstractUploadEngine::getUploadData() const
{
	return m_UploadData;
}
