#pragma once

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

#include <StormData/StormDataChangeNotifier.h>

#include <Foundation/Delegate/Delegate.h>
#include <Foundation/Document/Document.h>
#include <Foundation/Network/WebSocket.h>
#include <Foundation/PropertyMeta/PropertyFieldDatabase.h>

#include <DocumentServer/DocumentServerMessages.refl.h>


using DocumentOutputDelegate = Delegate<void, DocumentServerMessageType, const std::string &>;

class DocumentEditorBase : public QWidget
{
  Q_OBJECT
public:
  DocumentEditorBase(PropertyFieldDatabase & property_db, const std::string & root_path, DocumentOutputDelegate && output_delegate, QWidget *parent = nullptr);
  virtual ~DocumentEditorBase();

  void QueueChange(const ReflectionChangeNotification & change);
  void QueueAddLink(czstr source_document, czstr remote_path, czstr local_path);
  void QueueRemoveLink(czstr source_document, czstr remote_path, czstr local_path);
  void BeginTransaction();
  void CommitQueuedChanges();

  void GotServerChangeOkay();
  void LinksChanged(std::string link_data);

  virtual void GotDocumentChange(ReflectionChangeNotification & change);
  virtual void GotDocumentStateChange(DocumentState state, const std::string & document_data);

  virtual void ApplyServerChange(const ReflectionChangeNotification & change) = 0;
  virtual void ApplyLocalChange(const ReflectionChangeNotification & change) = 0;
  virtual void UpdateLinks() = 0;
  virtual void SyncHotSpots() = 0;

  void DocumentChangeComplete();

protected:

  virtual void ClearExistingContent();

private:

  void SetupDocumentStateMessage();
  void AddHotSpot(const ReflectionChangeNotification & change);

protected:
  PropertyFieldDatabase & m_PropertyDb;
  const std::string & m_RootPath;

  DocumentState m_State;
  DocumentOutputDelegate m_OutputDelegate;

  std::vector<std::string> m_HotSpots;
  std::vector<DocumentLink> m_Links;
  std::vector<std::pair<int, std::string>> m_PendingChanges;

  int m_UnacknowledgedChanges;

  bool m_IgnoreChanges;

private:

  bool m_TransactionEnabled;

  QLabel * m_DocumentErrorLabel;
  QGridLayout * m_DocumentErrorLayout;
};
