#include "pch.h"
#include "CreateKeyCommand.h"
#include "Registry.h"

CreateKeyCommand::CreateKeyCommand(PCWSTR path, PCWSTR name, AppCommandCallback<CreateKeyCommand> cb) : RegAppCommandBase(L"Create Key", path, name, cb) {
}

bool CreateKeyCommand::Execute() {
    auto key = Registry::OpenKey(_path, KEY_CREATE_SUB_KEY);
    if (!key)
        return false;
   
    CRegKey newKey;
    DWORD disp;
    auto error = newKey.Create(key, _name, nullptr, 0, KEY_READ | KEY_WRITE, nullptr, &disp);
    if (error == ERROR_SUCCESS) {
        return InvokeCallback(true);
    }
    return false;
}

bool CreateKeyCommand::Undo() {
    CRegKey key = Registry::OpenKey(_path, KEY_ENUMERATE_SUB_KEYS | DELETE | KEY_QUERY_VALUE);
    if(!key)
        return false;

    return ERROR_SUCCESS == ::RegDeleteTree(key, _name) ? InvokeCallback(false) : false;
}