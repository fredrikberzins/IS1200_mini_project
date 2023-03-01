# sleep.S 
#	Written by
#	Felix Bergqvist Widström
#	and Fredrik Berzins
#	(2023)
#	Modefied version of delay from is1200 lab 1

.global sleep

sleep:
	move 	$t0, $a0			# Set $t1, $a0			
	
	while_loop:						# While loop 
		ble	$t0, $zero, done		# Exit while loop
		nop
		sub	$t0, $t0, 1 			# Increment ms: ms = ms - 1 (ms--)
		
	li	$t1, 0					# Reset i: i = 0
						
	for_loop:					# For loop
		bge	$t1, 3799, while_loop	# 3799 is the constant nedded for the loop to take 1ms every time.
		nop
		addi	$t1, $t1, 1			# Increment i: i = i + 1 (i++)
		j	for_loop				# Restart the loop
		nop
			
	done:	
		jr	$ra	
		nop