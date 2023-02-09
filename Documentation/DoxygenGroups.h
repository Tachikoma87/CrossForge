/**
\defgroup Core Core
\brief Essential core classes



\defgroup AssetIO Asset Import/Export
\brief Classes to load and store external data

\defgroup Graphics Graphics
\brief Graphical capabilities of the CrossForge graphics framework.

\defgroup Actors Actors
\ingroup Graphics
\brief Actors that can be readily used.

\defgroup Camera Camera
\ingroup Graphics
\brief Classes related to camera and viewing

\defgroup AnimationController AnimationController
\ingroup Graphics
\brief Classes related to control the animation of an actor.

\defgroup Lights Lights
\ingroup Graphics
\brief Classes that enlighten your scene

\defgroup SceneGraph Scene Graph
\ingroup Graphics
\brief Organizes your scene



\defgroup Input Input Devices
\brief Input devices such as keyboard, mouse, gamepad

\defgroup Math Math
\brief Mathematics support

\defgroup Utility Utility
\brief Miscellaneous utility classes


\defgroup GUI GUI
\brief These classes form the basis of a simple GUI implementation.

For most simple use cases a developer will probably only need to
interact the the main class, CForge::GUI, and the few widgets,
it will return to you

Generally, in order to use the GUI, you should always create an
instance of CForge::GUI first. Then you can use its methods to
create widgets, or create some of your own. During the program's
render loop, two methods should be queried each frame:
CForge::GUI::processEvents and CForge::GUI::render. That is to
make the GUI aware of user input and to draw it onto the framebuffer
respectively. Once the program terminates, clean up should be
handled by CForge::GUI's destructor automatically, widgets
generally don't need to be destroyed manually if they were in
use within the GUI.

In order to best faciliate callbacks into the program code, it's
suggested to structure your program as class and have it inherit
the CForge::ITListener<GUICallbackObject> interface. Then you
can have everything that needs to be accessed by the callback
available as member variables. However, other means of providing
the callback function can also be used if deemed appropriate.
For more information on this kind of callback, see the documentation
of CForge::GUICallbackObject and CForge::FormWidget.



\defgroup Devices Hardware Devices
\brief Classes to use special hardware devices (e.g. sensors, actuators, etc.)


*/