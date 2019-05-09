/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
#include "BlackRoot/Pubc/Threaded IO Stream.h"
#include "BlackRoot/Pubc/Number Types.h"

#include "Aglaea/Pubc/Create Sluice.h"
#include "Aglaea/Pubc/Simple Performer.h"
#include "Aglaea/Pubc/Simple Disk IO.h"

#include "DemoAglaea/Pubc/Demo Disk Read.h"

using namespace DemoAglaea;

void Demo::BasicRead::Run()
{
    using cout = BlackRoot::Util::Cout;

    Aglaea::Util::SimplePerformer local;
    Aglaea::Util::SimpleDiskIO    io;

    struct temp_data {
        Aglaea::Util::IRAM *file_1, *file_2;
        Aglaea::Util::RAMDesc out_desc;
        std::string combined_files;
    } data;
    using cx = Aglaea::CreateSluice<temp_data>::InnerContext;

    auto * sluice = Aglaea::CreateSluice<temp_data>()
        .using_memory_ref(&data)

        .with(&local)
          .perform([](cx*) {
                cout{} << "Hello! " << std::this_thread::get_id();
            })

        .with(&io)
          .disk_path_to_ram("test 1.txt", &(data.file_1))

        .with(&local)
          .upon_fail_perform([](cx*) {
                cout{} << "Error during test 1! " << std::this_thread::get_id();
            })
          .perform([](cx*) {
                cout{} << "Success, read test 1 to ram " << std::this_thread::get_id();
            })

        .with(&io)
          .disk_path_to_ram("test 2.txt", &(data.file_2))

        .with(&local)
          .upon_fail_perform([](cx*) {
                cout{} << "Error during test 2! " << std::this_thread::get_id();
            })
          .perform([](cx * context) {
                cout{} << "Combining files... " << std::this_thread::get_id();
                temp_data & data = context->data;
                data.combined_files.reserve(0);
                data.combined_files.reserve(data.file_1->desc.length + data.file_2->desc.length);
                data.combined_files.append((char*)data.file_1->desc.data, data.file_1->desc.length);
                data.combined_files.append((char*)data.file_2->desc.data, data.file_2->desc.length);
                data.file_1->release();
                data.file_2->release();
                data.out_desc <= data.combined_files;
            })

        .with(&io)
          .ram_to_disk("out.txt", &(data.out_desc))

        .with(&local)
          .upon_fail_perform([](cx*) {
                cout{} << "Error during writing! " << std::this_thread::get_id();
            })
          .perform([](cx*) {
                cout{} << "Success, wrote file to disk " << std::this_thread::get_id();
            })
    .done_after_success();
}