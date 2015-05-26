# Fast Pupillary Fatique Waves Detection
Real-time pupillary fatique wave detection using OpenCV &amp; C++. This is our Computer Vision course project. **NOTICE: project uncompleted for now**

## What is it?
We use it to detect your pupillary diameter waves. Spontaneous waves of pupillary size and tiredness of human body are correlated. (`LOWENSTEIN O,FEINBERG R,LOEWENFELD I E. Pupillary movements during acute and chronic fatigue[J].Invest Ophthalmol Vis Sci,1963,2(2):138-157.`  )

Also referring to this paper (`Lüdtke H, Wilhelm B, Adler M, et al. Mathematical procedures in data recording and processing of pupillary fatigue waves[J]. Vision research, 1998, 38(19): 2889-2896.`) we are trying to apply the analysis of PUI( Pupillary Unrest Index) in a more effective way.

## Build and Language
How to use it: create a project in your vs, add OpenCV environment and add codes.

However, please notice:

* C++11 is used, so you might need a latest C++ compiler. The IDE I used is Visual Studio 2013. I advise you not use earlier IDE for you might have problems compiling it. 
* And OpenCV version is 2.4.11.
* windows.h is needed. I will try to remove it to support more platforms.
* Using Visual Leak Detector (2.4RC) as memory leak detector. However, it seems that it cannot detect all leaks in multi-thread mode.

## Files

### Multithreads Folder
Codes in this folder is the final version of our project.
**Notice: Codes in this folder will not need explicit IDE. Only OpenCV environemnt needed.**

* `main.cpp`: program entrance.
* `includes.h`: include all headers.
* `ThreadController.h/cpp`: controls a thread and its method. Modify your actions within ThreadController::runLoopControl() method.
* `ThreadPool.h/cpp`: a thread pool. Also it is a singleton - but not use it directly. Use SingletonGrabber to access the instance.

* `SingletonGrabber.h`: encapsulate a static singleton instance to realize singleton design pattern.
* `haarcascade_eye_tree_eyeglasses.xml`: classifier you can find from OpenCV.

### Procedure Folder
Codes in this folder displays how our algorithms processing an image.

* `main.cpp`: program entrance and processing this [image](http://www.nipic.com/show/1/11/fe8b096e1238b796.html).
* `includes.h`: include all headers.
* `haarcascade_eye_tree_eyeglasses.xml`: classifier you can find from OpenCV.
