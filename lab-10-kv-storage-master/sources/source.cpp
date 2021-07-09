// Copyright 2020 Your Name <your_email>

#include <header.hpp>


KVS::KVS(std::string log, unsigned count, std::string out):
    _log_level(std::move(log)),
    _thread_count(count),
    _output(std::move(out)) {
    options.create_if_missing = true;     // "создать БД, если не существует"
    if (_output.empty()) _output = "/home/ikarugao/Desktop/Study/C++LABS/MY/10/lab-10-kv-storage-master/newDB";
//    if (!first_db) delete first_db;
//    if (!second_db) delete second_db;
}

KVS::~KVS() {
    Status s;
    for (auto handle : first_handles) {
        first_db->DestroyColumnFamilyHandle(handle);
        assert(s.ok());
    }
    delete first_db;
    for (auto handle : second_handles) {
        second_db->DestroyColumnFamilyHandle(handle);
        assert(s.ok());
    }
    delete second_db;
}

void KVS::full_open(DB *db) {
    DB::ListColumnFamilies(options, input, &string_families);
    for (auto& i : string_families){
        column_families.emplace_back(i, ColumnFamilyOptions());
    }
    Status status = DB::
        Open(options, input, column_families, &first_handles, &db);
    assert(status.ok());
}

void KVS::circle() {
    log_setup();
    full_open(first_db);
    add_column_families(second_db);
    Status status = DB::
        Open(options, _output, column_families, &second_handles, &second_db);
    assert(status.ok());
    writer();
    printer(first_db, first_handles);
    std::cout << std::endl;
    printer(second_db, second_handles);
}

void KVS::printer(DB* db, const std::vector<ColumnFamilyHandle*>& handle) {
    for (auto i : handle){
        //std::cout << "Family: "<< i->GetName() << std::endl;
        Iterator* it = db->NewIterator(ReadOptions(), i);
        for (it->SeekToFirst(); it->Valid(); it->Next()) {
            logging(i->GetName(), it->key().ToString(), it->value().ToString());
        }
    }
}

void KVS::add_column_families(DB *db) {
    Status status = DB::Open(options, _output, &db);
    assert(status.ok());
    for (const auto& j : column_families){
        if (j.name == "default") continue;
        ColumnFamilyHandle* cf;
        Status s = db->
            CreateColumnFamily(ColumnFamilyOptions(), j.name, &cf);
        assert(s.ok());
    }
    delete db;
}

void KVS::writer() {
    int index = 0;
    for (auto i : second_handles){
        Iterator* it1 = first_db->
            NewIterator(ReadOptions(), first_handles[index]);
        for (it1->SeekToFirst(); it1->Valid(); it1->Next()) {
            auto hash = picosha2::
                hash256_hex_string(it1->key().ToString() +
                it1->value().ToString());
            Status s = second_db->
                Put(WriteOptions(), i, it1->key().ToString(), hash);
            assert(s.ok());
            //logging(i->GetName(), it1->key().ToString(), hash);
        }
        index++;
    }
}

void KVS::log_setup() {
    boost::shared_ptr< logging::core > core = logging::core::get();

    boost::shared_ptr< boost::log::sinks::text_file_backend > backend =
            boost::make_shared< boost::log::sinks::text_file_backend >(
                    keywords::file_name = "/tmp/log",
                    keywords::rotation_size = 5 * 1024 * 1024,
                    keywords::format = "%Message%");

    typedef boost::log::sinks::synchronous_sink
            < boost::log::sinks::text_file_backend > sink_t;
    boost::shared_ptr< sink_t > sink(new sink_t(backend));
//    sink ->set_filter(logging::trivial::severity >= logging::trivial::info);
    core->add_sink(sink);
    logging::add_console_log
            (
                    std::cout,
                    logging::keywords::format =
                            "%Message%");
}

void KVS::logging(std::string family, std::string key, std::string value) {
    if (_log_level == "trace"){
        BOOST_LOG_TRIVIAL(trace) << "Family: "<< family << " key: "
        << key << " value: " << value;
    }else{
        BOOST_LOG_TRIVIAL(info) << "Family: "<< family << " key: "
        << key << " value: " << value;
    }
}
