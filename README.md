drush-m2c
=========

Converter from a Drush makefile to composer.json

Usage
-----

Install this in your .drush folder:

    $ git clone git@github.com:jpstacey/drush-m2c.git ~/.drush/m2c
    $ drush cc drush

Then use the m2c command, specifying a makefile:

    $ drush m2c path/to/drush-m2c/make_to_composer.drush path/to/makefile.make > path/to/composer.json

This will convert the makefile into a composer.json on standard output, so
as you can see above we're piping that to an actual JSON file.

It's very much in alpha and you should only use it at your own risk. You're
very much likely to have to edit the compopser.json afterwards.
