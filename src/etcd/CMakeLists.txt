add_library(etcd-cpp-client SHARED ../../proto/kv.pb.cc ../../proto/auth.pb.cc ../../proto/rpc.pb.cc ../../proto/rpc.grpc.pb.cc client.cpp sclient.cpp concurrentmap.cpp)
set_property(TARGET etcd-cpp-client PROPERTY CXX_STANDARD 11)

target_link_libraries(etcd-cpp-client boost_system boost_thread protobuf grpc++)

install (TARGETS etcd-cpp-client DESTINATION lib)
install (FILES client.h sclient.h concurrentmap.h
               DESTINATION include/etcd)

