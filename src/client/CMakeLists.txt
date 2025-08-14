aux_source_directory(../server/model CLIENT_SRC)
aux_source_directory(../server/db CLIENT_SRC)
aux_source_directory(. CLIENT_SRC)

add_executable(ChatClient ${CLIENT_SRC})
target_link_libraries(ChatClient PRIVATE muduo_net  muduo_base mysqlclient mysqlcppconn pthread)