# Updates
Update record file

## Version 1
**Version 1.0**
* Fundamental structures 
* Use multi-threads to display

**Version 1.1**
* Now it can display something from your camera
* Use advanced singleton design pattern.
* No memory leaks now - using Visual Leak Detector 2.4RC.

**Version 1.2**
* It can detect your eyes now without any crash.
* Use a trick to keep ThreadPool and ThreadController cannot be inherited.

**Version 1.3**
* No functionality added but adjust the structure.
* Allow ThreadController can be inherited.

**Version 1.3.1**
* Fix a bug: delete default ctor of ThreadController

**Version 1.3.2**
* `Uninheritable.h` deprecated. Use final instead.
* Reconstruct the SingletonGrabber

**Version 1.4**
* Reconstruct all codes without no leaks.

**Version 1.4.1**
* Transform all codes into Visual Studio 2013.
* Add the Procedure folder to display algorithms.

**Version 1.5**
* In `Procedure` folder, now we can detect pupillary diameter from an image.
* Add `CVEx.h` providing extended methods.

**Version 1.5.1**
* Small adjustments.
* Bug detected: codes in multithreads folder will have two memory leaks (CvCapture in ThreadPool::runLoop()) at x64 debug mode but no leaks in win32 debug mode.

**Version 1.5.2**
* Optimize the codes.

**Version 1.5.3**
* Add codes of detecting the diameter to multithreads codes.

**Version 1.6**
* Reconstruct the thread structure: provide more support for continuous thread pool and queued thread pool.

**Version 1.6.1**
* Add more functionality for SingletonGrabber.
* Put all threads file into another folder.

**Version 1.6.2**
* Small adjustments

**Version 1.6.3**
* Redesign the SingletonGrabber class. Now we can use it with more efficiency.
* Add Initialize() method for ThreadPool and ThreadController to initialize member variables while enabling polymorphism. However, that means you have to call it immediately after ctors.
* Pass data from ThreadJob to thread pool.
* Use std::condition_variable to make the thread wait while there is no job. We have about 30% efficiency promotion with this feature.

**Version 1.6.4**
* Add a function called `analyze` in the PDThreadPool class.  It will perform interpolations, perform FFT, get the variance and generate PUI. For now only interpolation algorithm lacked.
* Use a new pupillary diameter detection algorithm.

**Version 1.6.5**
* Provide a concise pupilla position and diameter detection method. Really concise.
