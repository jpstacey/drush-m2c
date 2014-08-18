drush-m2c
=========

Converter from a Drush makefile to composer.json

WARNING
-------

It's very much in alpha and you should only use it at your own risk. You're
very much likely to have to edit the compopser.json afterwards.

Usage
-----

Install this in your .drush folder:

    $ git clone git@github.com:jpstacey/drush-m2c.git ~/.drush/m2c
    $ drush cc drush

Then use the m2c command, specifying a makefile:

    $ drush m2c path/to/drush-m2c/make_to_composer.drush path/to/makefile.make > path/to/composer.json

This will convert the makefile into a composer.json on standard output, so
as you can see above we're piping that to an actual JSON file.

What Composer doesn't do
------------------------

A limitation of installers is that it doesn't cope with building a runnable
core-plus-contrib: your files will be in the following locations:

* modules/
* themes/
* profiles/
* libraries/
* vendor/drupal/drupal/ <- core

You will need to manually rearrange these files to get your build to work.
However, this is a task that could be very much suited to Drush instance:

https://www.drupal.org/project/drush_instance

There's an open issue to handle this in a future release.

Another thing Composer doesn't do (yet) is apply patches, post-install. This
is an open issue on this project but isn't likely to be solved at this level
alone.
