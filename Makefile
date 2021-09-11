
tests: tests_lib tests_teseo

tests_lib:
	cd lib; bundle exec arduino_ci.rb --skip-examples-compilation

tests_teseo:
	cd teseo; bundle exec arduino_ci.rb --skip-examples-compilation

simulation:
	cd simulator; bundle exec arduino_ci.rb --skip-examples-compilation
