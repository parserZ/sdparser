include_directories (./utils)

set (clnndep_SRC
     ArcEager.cpp
     ArcEager.h
     Classifier.cpp
     Classifier.h
     Config.cpp
     Config.h
     Configuration.cpp
     Configuration.h
     Dataset.cpp
     Dataset.h
     DependencyParser.cpp
     DependencyParser.h
     DependencySent.cpp
     DependencySent.h
     DependencyGraph.cpp
     DependencyGraph.h
     fastexp.h
     nndep.cpp
     ParsingSystem.cpp
     ParsingSystem.h
     SecondHead.h
     ThreadPool.h
     time.h
     Util.h)

add_executable (eagernndep ${clnndep_SRC})

