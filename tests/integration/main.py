import sys
sys.path.append( "utils" ) # Adding utils directory to the import path

import re
import os
import unittest

# Find which test to run
try:
  filename = sys.argv[sys.argv.index("--file") + 1]
  if filename.startswith("-"): raise Exception # Protection against bad syntax
except: # Load all tests
  filename = ""

# Then launch our test according to what user asked
if filename == "":
  print "Suite discovering tests"
  suite = unittest.TestLoader().discover( "." )
else:
  print "Suite loading tests from file: %s" % filename
  suite = unittest.TestLoader().discover( ".", filename )

print "Discovered %i tests" % suite.countTestCases()

if suite.countTestCases() > 0:
  unittest.TextTestRunner().run(suite)
else:
  print "No test to run"