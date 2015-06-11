# Fast Pupillary Fatique Waves Detection
Real-time pupillary fatique wave detection using OpenCV &amp; C++. This is our Computer Vision course project. **NOTICE: project uncompleted for now**

## What is it?
We use it to detect your pupillary diameter waves. Spontaneous waves of pupillary size and tiredness of human body are correlated. (`LOWENSTEIN O,FEINBERG R,LOEWENFELD I E. Pupillary movements during acute and chronic fatigue[J].Invest Ophthalmol Vis Sci,1963,2(2):138-157.`  )

Also referring to this paper (`Lüdtke H, Wilhelm B, Adler M, et al. Mathematical procedures in data recording and processing of pupillary fatigue waves[J]. Vision research, 1998, 38(19): 2889-2896.`) we are trying to apply the analysis of PUI( Pupillary Unrest Index) in a more effective way.

## Build and Language
How to use it: create a project in your vs, add OpenCV environment and add codes.

However, please notice:

* Use C++11 standards and IDE is VS2013.
* And OpenCV version is 2.4.11.
* Visual Leak Detector (2.4RC) as memory leak detector.

## Files

### Multithreads Folder
Codes in this folder is the final version of our project.

* `main.cpp`: program entrance.
* `includes.h`: include all headers.
* `nullref.h`: provide nullref realization.
* `CVEx.h`: extend OpenCV algorithms and used in main.cpp
* `PDThreadController.h/cpp`: controls a thread and its method. Modify your actions within ThreadController::runLoop() method.
* `PDThreadPool.h/cpp`: a thread pool. Also it is a singleton - but not use it directly. Use SingletonGrabber to access the instance.
* `PDThreadJob.h/cpp`: algorithm processing the image and get the diameter.
* `SingletonGrabber.h`: encapsulate a static singleton instance to realize singleton design pattern.
* `haarcascade_eye_tree_eyeglasses.xml`: classifier you can find from OpenCV.
* `/Threads` under this folder there is a suit to use multithreads functionality of C++11 with continuous pool and queued pool.

### Procedure Folder
Codes in this folder displays how our algorithms processing an image.
**Notice: Codes in this folder will not need explicit IDE. Only OpenCV environemnt needed.**

* `main.cpp`: program entrance and processing this [image](http://www.nipic.com/show/1/11/fe8b096e1238b796.html).
* `includes.h`: include all headers.
* `CVEx.h`: extend OpenCV algorithms and used in main.cpp
* `haarcascade_eye_tree_eyeglasses.xml`: classifier you can find from OpenCV.
