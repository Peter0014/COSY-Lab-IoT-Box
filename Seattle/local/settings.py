"""
<Program Name>
  settings.py

<Started>
  September 2010

<Author>
  Alex Hanson

<Purpose>
  A host-specific Django settings file that overrides the defaults provided in
  settings_base.py. This file should be specified as the Django settings file
  when running the website.
"""

import os

from custominstallerbuilder.settings_base import *


# This file serves to override the default settings provided in the
# settings_base.py file in the project root.


# Make this key a random string, and don't share it with anyone.
SECRET_KEY = 'Hallo321321**'

# Unless you are actively debugging, these should be set to False.
DEBUG = True

# During testing, you may want to use Django's built-in static file server.
SERVE_STATIC = True
STATIC_BASE = 'static/'

# The base domain on which the server operates.
BASE_URL = 'http://127.0.0.1:1025/'

# The root directory of the Custom Installer Builder.
PROJECT_URL = BASE_URL

# The location of static media.
MEDIA_URL = PROJECT_URL + 'static/'

# The locations of the customized installers created by this program.
CUSTOM_INSTALLER_URL = PROJECT_URL + 'static/installers/'

# The host/domain names that this Django site can serve. Usually matches
# the FQDN in your BASE_URL, but may include (sub-)domain wildcards.
# To implicitly allow serving on localhost for testing purposes, outcomment
# it and set DEBUG = True above.
ALLOWED_HOSTS = ['localhost', '127.0.0.1']
