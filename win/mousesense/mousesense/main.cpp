//
//  main.cpp
//  mousesense
//
//  Created by Winthrop Gillis on 4/14/18.
//  Copyright © 2018 Winthrop Gillis. All rights reserved.
//

#include <iostream>
#include <signal.h>
#include <atomic>
#include <thread>
#include <librealsense2/rs.hpp>
#include <librealsense2/rs_advanced_mode.hpp>
#include <streambuf>
#include <fstream>
#include <string>
#include "fileSaver.hpp"

atomic<bool> saving(true);
rs2::frame_queue queue(90);

void handle_interrupt(int sig) {
    printf("Program shutting down...\n");
    saving = false;
}

string load_json(string fname) {
    ifstream file(fname);
    string contents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return (string)contents;
}

void saving_thread() {
    FileSaver depth("depth.bin", true);
    FileSaver timeStamp("timestamp.txt", false);
    FileSaver frameNumber("framenumber.txt", false);
    
    while(saving) {
        rs2::frame fr;
        if (queue.poll_for_frame(&fr)) {
            depth.write(fr.as<rs2::video_frame>());
            timeStamp.write(fr.get_timestamp());
            frameNumber.writeFrameNumber(fr.get_frame_number());
        }
    }
    
}

int main(int argc, const char * argv[]) {
    const int width = 848;
    const int height = 480;
    const int fps = 90;
    
    rs2::context ctx;
    rs2::device_list devices = ctx.query_devices();
    rs2::device dev = devices.front();
    rs400::advanced_mode adv = dev.as<rs400::advanced_mode>();
    string params = load_json("d415paramset_1.json");
    adv.load_json(params);
    printf("%s", adv.serialize_json().c_str());
    
    // setup capturing the interrupt
    signal(SIGINT, &handle_interrupt);
    //signal(SIGKILL, &handle_interrupt);
    
    thread t(&saving_thread);
    t.detach();
    
    // define the realsense pipe
    rs2::pipeline pipe;
    
    // define the realsense configuration
    rs2::config config;

    config.enable_stream(RS2_STREAM_DEPTH, 0, width, height, RS2_FORMAT_Z16, fps);
    
    rs2::pipeline_profile profile = pipe.start(config);
    


//    dev.load_json(params);
    

    printf("Initialized\n");
    while(saving) {
        // capture stream and save it
        rs2::frameset frameset = pipe.wait_for_frames();
        rs2::depth_frame frame = frameset.get_depth_frame();

        queue.enqueue(frame);
    }
    
    return EXIT_SUCCESS;
}
//} catch (const rs2::error & e) {
//    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
//    return EXIT_FAILURE;
//} catch (const std::exception& e) {
//    std::cerr << e.what() << std::endl;
//    return EXIT_FAILURE;
//
//}
