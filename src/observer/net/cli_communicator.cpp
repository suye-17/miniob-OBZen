/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2023/06/25.
//

#include "net/cli_communicator.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "event/session_event.h"
#include "net/buffered_writer.h"
#include "session/session.h"
#include "common/linereader/line_reader.h"

#define MAX_MEM_BUFFER_SIZE 8192
#define PORT_DEFAULT 6789

using common::MiniobLineReader;

const std::string LINE_HISTORY_FILE = "./.miniob.history";

RC CliCommunicator::init(int fd, unique_ptr<Session> session, const string &addr)
{
  RC rc = PlainCommunicator::init(fd, std::move(session), addr);
  if (OB_FAIL(rc)) {
    LOG_WARN("fail to init communicator", strrc(rc));
    return rc;
  }

  if (fd == STDIN_FILENO) {
    write_fd_ = STDOUT_FILENO;
    delete writer_;
    writer_ = new BufferedWriter(write_fd_);

    MiniobLineReader::instance().init(LINE_HISTORY_FILE);

    const char delimiter = '\n';
    send_message_delimiter_.assign(1, delimiter);

    fd_ = -1;  // 防止被父类析构函数关闭
  } else {
    rc = RC::INVALID_ARGUMENT;
    LOG_WARN("only stdin supported");
  }
  return rc;
}

RC CliCommunicator::read_event(SessionEvent *&event)
{
  event                  = nullptr;
  const char *prompt_str = "miniob > ";
  std::string command    = MiniobLineReader::instance().my_readline(prompt_str);
  if (command.empty()) {
    return RC::SUCCESS;
  }

  if (common::is_blank(command.c_str())) {
    return RC::SUCCESS;
  }

  if (MiniobLineReader::instance().is_exit_command(command)) {
    exit_ = true;
    return RC::SUCCESS;
  }

  event = new SessionEvent(this);
  event->set_query(command);
  return RC::SUCCESS;
}

RC CliCommunicator::write_result(SessionEvent *event, bool &need_disconnect)
{
  RC rc = PlainCommunicator::write_result(event, need_disconnect);

  need_disconnect = false;
  return rc;
}

CliCommunicator::~CliCommunicator() { LOG_INFO("Command history saved to %s", LINE_HISTORY_FILE.c_str()); }
