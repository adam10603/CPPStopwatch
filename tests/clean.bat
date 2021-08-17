@echo off
move out_vs\.gitignore . > NUL
del /s /q out_vs\* > NUL
del /s /q tmp_vs\* > NUL
rmdir /s /q tmp_vs\tests.tlog > NUL
move .gitignore out_vs\ > NUL
copy out_vs\.gitignore tmp_vs\ > NUL