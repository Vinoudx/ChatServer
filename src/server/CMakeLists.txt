aux_source_directory(. SRC_LIST)
aux_source_directory(./db SRC_LIST)
aux_source_directory(./model SRC_LIST)
aux_source_directory(./redis SRC_LIST)

add_executable(ChatServer ${SRC_LIST})
target_link_libraries(ChatServer PRIVATE muduo_net  muduo_base mysqlclient mysqlcppconn hiredis pthread)