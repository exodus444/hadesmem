// Copyright Joshua Boyce 2010-2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#include "hadesmem/module_list.hpp"

#include <boost/assert.hpp>

#include "hadesmem/error.hpp"
#include "hadesmem/module.hpp"
#include "hadesmem/process.hpp"

namespace hadesmem
{

ModuleIter::ModuleIter()
  : process_(nullptr), 
  back_(nullptr), 
  module_()
{ }

ModuleIter::ModuleIter(Process const& process, MODULEENTRY32 const& entry, 
  ModuleList* back)
  : process_(&process), 
  back_(back), 
  module_(new Module(process, entry))
{ }

ModuleIter::ModuleIter(ModuleIter const& other)
  : process_(other.process_), 
  back_(other.back_), 
  module_()
{
  if (other.module_)
  {
    module_.reset(new Module(*other.module_));
  }
  else
  {
    process_ = nullptr;
    back_ = nullptr;
  }
}

ModuleIter& ModuleIter::operator=(ModuleIter const& other)
{
  process_ = other.process_;
  back_ = other.back_;
  module_.reset(new Module(*other.module_));
  
  return *this;
}

const Module& ModuleIter::operator*() const
{
  return *module_;
}

const Module* ModuleIter::operator->() const
{
  return &*module_;
}

ModuleIter& ModuleIter::operator++()
{
  auto next = back_->Next();
  if (next)
  {
    module_.reset(new Module(*process_, *next));
  }
  else
  {
    process_ = nullptr;
    back_ = nullptr;
    module_.reset();
  }
  
  return *this;
}

ModuleIter ModuleIter::operator++(int)
{
  ModuleIter prev(*this);
  ++*this;
  return prev;
}

bool ModuleIter::equal(ModuleIter const& other) const
{
  return this->process_ == other.process_ && 
    this->back_ == other.back_ && 
    ((this->module_ && other.module_ && *this->module_ == *other.module_) || 
    (!this->module_ && !other.module_));
}

bool operator==(ModuleIter const& lhs, ModuleIter const& rhs)
{
  return lhs.equal(rhs);
}

bool operator!=(ModuleIter const& lhs, ModuleIter const& rhs)
{
  return !lhs.equal(rhs);
}

ModuleList::ModuleList(Process const& process)
  : process_(&process), 
  snap_(nullptr)
{
  snap_ = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, 
    process_->GetId());
  if (snap_ == INVALID_HANDLE_VALUE)
  {
    DWORD const last_error = GetLastError();
    BOOST_THROW_EXCEPTION(HadesMemError() << 
      ErrorString("CreateToolhelp32Snapshot failed.") << 
      ErrorCodeWinLast(last_error));
  }
}

ModuleList::~ModuleList()
{
  // WARNING: Handle is leaked if CloseHandle fails.
  BOOST_VERIFY(CloseHandle(snap_));
}

ModuleList::iterator ModuleList::begin()
{
  MODULEENTRY32 entry;
  ZeroMemory(&entry, sizeof(entry));
  entry.dwSize = sizeof(entry);
  
  if (!Module32First(snap_, &entry))
  {
    DWORD const last_error = GetLastError();
    BOOST_THROW_EXCEPTION(HadesMemError() << 
      ErrorString("Module32First failed.") << 
      ErrorCodeWinLast(last_error));
  }
  
  return iterator(*process_, entry, this);
}

ModuleList::iterator ModuleList::end()
{
  return iterator();
}

std::unique_ptr<MODULEENTRY32> ModuleList::Next()
{
  MODULEENTRY32 entry;
  ZeroMemory(&entry, sizeof(entry));
  entry.dwSize = sizeof(entry);
  
  if (!Module32Next(snap_, &entry))
  {
    if (GetLastError() == ERROR_NO_MORE_FILES)
    {
      return std::unique_ptr<MODULEENTRY32>();
    }
    else
    {
      DWORD const last_error = GetLastError();
      BOOST_THROW_EXCEPTION(HadesMemError() << 
        ErrorString("Module32Next failed.") << 
        ErrorCodeWinLast(last_error));
    }
  }
  
  return std::unique_ptr<MODULEENTRY32>(new MODULEENTRY32(entry));
}

}