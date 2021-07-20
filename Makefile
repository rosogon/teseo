
tests:
	cd lib; bundle exec arduino_ci.rb --skip-examples-compilation
	cd teseo; bundle exec arduino_ci.rb --skip-examples-compilation
