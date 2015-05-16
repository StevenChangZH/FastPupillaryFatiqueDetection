# PupilDetection
Real-time pupil detection using OpenCV &amp; C++. This is our Computer Vision course project. **NOTICE: project uncompleted**

## What is it?
We use it to detect your pupillary diameter waves. Spontaneous waves of pupillary size and tiredness of human body are correlated. (*LOWENSTEIN O,FEINBERG R,LOEWENFELD I E. Pupillary movements during acute and chronic fatigue[J].Invest Ophthalmol Vis Sci,1963,2(2):138-157. *) Also referring to this paper (*Lüdtke H, Wilhelm B, Adler M, et al. Mathematical procedures in data recording and processing of pupillary fatigue waves[J]. Vision research, 1998, 38(19): 2889-2896.*) we are trying to apply the analysis of PUI( Pupillary Unrest Index) in a more effective way.

## Build and Language
How to use it: create a project in your vs, add OpenCV environment and add codes.

However, please notice:

* C++11 is used, so you might need a latest C++ compiler. The IDE I used is Visual Studio 2012. I advise you not use earlier IDE for you might have problems compiling it. Even though we know, VS do not support all C++11 features (at least, I cannot use default and delete functions).
* And OpenCV version is 2411.
* windows.h is needed. I will try to remove it to support more platforms.
* Using Visual Leak Detector (2.4RC) as memory leak detector. However, it seems that it cannot detect all leaks in multi-thread mode.

## Files

* `main.cpp`: program entrance.
* `stdafx.h`: include all headers.
* `ThreadController.h/cpp`: controls a thread and its method. Modify your actions within ThreadController::runLoopControl() method.
* `ThreadPool.h/cpp`: a thread pool. Also it is a singleton - but not use it directly. Use SingletonGrabber to access the instance.

* `SingletonGrabber.h`: encapsulate a static singleton instance to realize singleton design pattern.
* `UnInheritable.h`: encapsulate a virtual base class to keep ThreadPool cannot be inherited.
* `haarcascade_eye_tree_eyeglasses.xml`: classifier you can find from OpenCV.