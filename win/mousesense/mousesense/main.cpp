//
//  main.cpp
//  mousesense
//
//  Created by Winthrop Gillis on 4/14/18.
//  Copyright © 2018 Winthrop Gillis. All rights reserved.
//

#include <iostream>
#include <signal.h>

#include <librealsense2/rs.hpp>

void handle_interrupt(int sig) {
    printf("Program shutting down...\n");
    exit(0);
}

int main(int argc, const char * argv[]) try {
    const int width = 848;
    const int height = 480;
    const int fps = 90;
    
    // setup capturing the interrupt
    signal(SIGINT, &handle_interrupt);
    
    // define the realsense pipe
    rs2::pipeline pipe;
    // define the realsense configuration
    rs2::config config;
    //config.enable_stream(RS2_STREAM_DEPTH, width, height);
    config.enable_stream(RS2_STREAM_DEPTH, 0, width, height, RS2_FORMAT_Z16, fps);
    pipe.start(config);
    int i = 0;
    printf("Initialized\n");
    while(true) {
        // capture stream and save it
        rs2::frameset frameset = pipe.wait_for_frames();
        rs2::depth_frame frame = frameset.get_depth_frame();
        printf("%d\n", i);
        i++;
    }
    
    return EXIT_SUCCESS;
} catch (const rs2::error & e) {
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
} catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;

}
