
#include "Foundation/FileSystem/Path.h"

#include "Runtime/RuntimeCommon.h"
#include "Runtime/DocumentResource/DocumentResourceManager.h"

extern DocumentResourceLoader * g_ResourceLoader;
DocumentResourceManager g_DocumentResourceManager;

DocumentResourceManager::DocumentResourceManager() :
  m_PendingDocumentLoad(0)
{

}

NotNullPtr<DocumentResourceBase> DocumentResourceManager::LoadDocumentResource(czstr file_path, std::unique_ptr<DocumentResourceBase>(*ResourceCreator)(Any &&, uint64_t))
{
  uint64_t path_hash = crc64(file_path);

  auto itr = m_Documents.find(path_hash);
  if (itr == m_Documents.end())
  {
    m_PendingDocumentLoad = path_hash;
    m_PendingDocumentLoaded = false;

    auto load_data = g_ResourceLoader->LoadResource(file_path,
      [](uint64_t path_hash, NullOptPtr<std::string> resource_data, void * user_ptr)
      { 
        auto p_this = static_cast<DocumentResourceManager *>(user_ptr);
        p_this->HandleResourceLoadResult(path_hash, resource_data);
      }, this);

    auto resource = ResourceCreator(std::move(load_data), path_hash);
    resource->m_ResourceManager = this;

    auto result = m_Documents.emplace(std::make_pair(path_hash, std::move(resource)));

    m_PendingDocumentLoad = 0;
    if (m_PendingDocumentLoaded)
    {
      HandleResourceLoadResult(path_hash, m_PendingDocumentData.size() > 0 ? &m_PendingDocumentData : nullptr);
      m_PendingDocumentData.clear();
    }

    return result.first->second.get();
  }
  else
  {
    return itr->second.get();
  }
}

void DocumentResourceManager::HandleResourceLoadResult(uint64_t path_hash, NullOptPtr<std::string> resource_data)
{
  if (path_hash == m_PendingDocumentLoad)
  {
    if (resource_data)
    {
      m_PendingDocumentData = *resource_data;
    }
    else
    {
      m_PendingDocumentData.clear();
    }
    
    m_PendingDocumentLoaded = true;
    return;
  }

  auto itr = m_Documents.find(path_hash);
  if (itr == m_Documents.end())
  {
    return;
  }

  if (resource_data)
  {
    itr->second->OnDataLoadComplete(*resource_data);
  }
  else
  {
    itr->second->OnDataLoadComplete("{}");
  }

  itr->second->CallAssetLoadCallbacks();
}

void DocumentResourceManager::DestroyDocument(uint64_t path_hash)
{
  auto itr = m_Documents.find(path_hash);
  if (itr == m_Documents.end())
  {
    return;
  }

  g_ResourceLoader->UnloadResource(path_hash, std::move(itr->second->m_LoadData));
  m_Documents.erase(itr);
}
