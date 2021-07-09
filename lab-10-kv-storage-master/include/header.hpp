// Copyright 2020 Your Name <your_email>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_

#include <vector>
#include <string>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>
#include <iostream>
#include <picosha2.h>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks.hpp>

using rocksdb::DB;
using rocksdb::DBOptions;
using rocksdb::ColumnFamilyOptions;
using rocksdb::ReadOptions;
using rocksdb::WriteOptions;
using rocksdb::Options;
using rocksdb::Status;
using rocksdb::ColumnFamilyDescriptor;
using rocksdb::ColumnFamilyHandle;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;

const char input[] = "/home/ikarugao/Desktop/Study/C++LABS/MY/10/lab-10-kv-storage-master";

class KVS{
public:
    KVS(std::string log, unsigned count, std::string out);
    ~KVS();
    void circle();
    void full_open(DB *db);
    void add_column_families(DB *db);
    void printer(DB *db, const std::vector
         <ColumnFamilyHandle*>& handle);
    void writer();
    void log_setup();
    void logging(std::string family, std::string key, std::string value);
private:
    DB* first_db;     // переменная для БД
    DB* second_db;
    Options options;    // переменная для опций
    std::vector<std::string> string_families;
    std::vector<ColumnFamilyDescriptor> column_families;
    std::vector<ColumnFamilyHandle*> first_handles;
    std::vector<ColumnFamilyHandle*> second_handles;

    std::string _log_level;
    unsigned _thread_count;
    std::string _output;
};

#endif // INCLUDE_HEADER_HPP_
