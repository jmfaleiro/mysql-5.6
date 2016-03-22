/*
   Copyright (c) 2015, Facebook, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
#pragma once

#include "rocksdb/listener.h"

namespace myrocks {

class Table_ddl_manager;

class Rdb_event_listener : public rocksdb::EventListener
{
 public:
  explicit Rdb_event_listener(Table_ddl_manager* ddl_manager) :
      m_ddl_manager(ddl_manager) {
  }

  void OnCompactionCompleted(
    rocksdb::DB *db, const rocksdb::CompactionJobInfo& ci) override;
  void OnFlushCompleted(
    rocksdb::DB* db, const rocksdb::FlushJobInfo& flush_job_info) override;

 private:
  Table_ddl_manager* m_ddl_manager;
};

}  // namespace myrocks