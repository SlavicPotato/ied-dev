echo | set /p dummyName=#define GIT_CUR_COMMIT > gitparams.h
git rev-parse --short HEAD >> gitparams.h

echo | set /p dummyName=#define GIT_BRANCH >> gitparams.h
git rev-parse --abbrev-ref HEAD >>  gitparams.h