import pyrealsense2 as rs

def main():
    try:
        pipeline = rs.pipeline()
        pipeline.start()
        
        i = 0
        while True:
            frames = pipeline.wait_for_frames()
            depth = frames.get_depth_frame()
            if not depth:
                print('Oh no')
            print(i)
            i += 1

    except Exception as e:
        print(e)

if __name__ == '__main__':
    main()
