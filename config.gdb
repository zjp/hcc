set breakpoint pending on
set confirm off
file ./holeycc
break holeyc::Err::report
commands
	where
end
break holeyc::ToDoError::ToDoError
commands
	where
end
break holeyc::InternalError::InternalError
commands
	where
end

define t2
  set args p2_tests/$arg0.holeyc -p --
  run
end

define t3
  set args p3_tests/$arg0.holeyc -n --
  run
end

define t4
  set args p4_tests/$arg0.holeyc -c
  run
end

define t5
  set args p4_tests/$arg0.holeyc -a --
  run
end
