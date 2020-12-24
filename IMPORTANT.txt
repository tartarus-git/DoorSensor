Found this weird thing. I don't know if it's a bug in Raspbian or if I'm just dumb, but moving Folder/. into New_Folder doesn't work for some reason.
I get that it's a sort of redundant expression, but it still should work right?
It comes up with a an error saying device or resource is busy. Really weird. If this is a bug, consider reporting it.


OTHER IMPORTANT THING:

The -l thing in gcc always dynamically links to my knowledge, unless you specify the -static argument with it.
Also, the link order is important so remember that in case you come across any unexplainable bugs.
Also, it turns out that you have to link to pthread(posix thread) when using thread in c++ on linux, because thread is compatible with linux, but it need the pthread library
linked.
Also, it is recommended for some reason to write -l and the library together. The option to write them apart is just for POSIX compliance.
