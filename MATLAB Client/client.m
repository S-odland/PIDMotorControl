function client(port)

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

fprintf('opening port %s....\n',port);

mySerial = serial(port, 'BaudRate',230400,'Timeout',120);
fopen(mySerial);
clean = onCleanup(@()fclose(mySerial));

operatingModes = {'IDLE','PWM','ITEST','HOLD','TRACK'};
modeSet = [0,1,2,3,4];

M = containers.Map(modeSet,operatingModes);

has_quit = false;
while ~has_quit
    fprintf('PIC32 MOTOR DRIVER INTERFACE\n\n');
    fprintf(' a: Read current sensor (ADC counts)     b: Read current sensor (mA)\n c: Read encoder (counts)                d: Read encoder (deg)\n e: Reset encoder                        f: Set PWM (-100 to 100)\n g: Set current gains                    h: Get current gains\n i: Set position gains                   j: Get position gains\n k: Test current control                 l: Go to angle (deg)\n m: Load step trajectory                 n: Load cubic trajectory\n o: Execute trajectory                   p: Unpower the motor\n q: Quit client                          r: Get mode\n');
    selection = input('\nENTER COMMAND: ', 's');
    
    fprintf(mySerial, '%c\n', selection);
    
    switch selection
        case 'a'
            adcCount = fscanf(mySerial, '%d');
            fprintf('The current sensor (ADC counts) is %d.   ',adcCount);
        case 'b'
            amps = fscanf(mySerial, '%d');
            fprintf('The current sensor (mA) is %d.   ',amps);
        case 'c'
            counts = fscanf(mySerial,'%d');
            fprintf('The motor angle is %d counts.   ',counts);
        case 'd'
            angle = fscanf(mySerial,'%d');
            fprintf('The motor angle is at %d degrees.   ',angle);
        case 'e'
            reset = fscanf(mySerial,'%d');
            fprintf('The counts have been reset to %d counts.   ',reset);
        case 'f'
            pwm = input('Enter PWM (-100 to 100): ');
            fprintf(mySerial, '%d\n',pwm);
            if pwm<0
                resp = fscanf(mySerial, '%d');
                fprintf('PWM has been set to %d in the clockwise direction.   ',resp);
            end
            if pwm>0
                resp = fscanf(mySerial, '%d');
                fprintf('PWM has been set to %d in the counterclockwise direction.   ',resp);
            end
        case 'g'
            gains = input('Enter your desired Kp and Ki current gain values: ');
            fprintf(mySerial,'%d %d\n',gains);
            
            fprintf('Sending Kp = %d and Ki = %d to the current controller.   ',gains(1),gains(2));
        case 'h'
            gains1 = fscanf(mySerial, '%d %d');
            
            fprintf('The current controller is using Kp = %d and Ki = %d.   ',gains1(1),gains1(2));
        case 'i'
            pgains = input('Enter your desired Kp, Ki and Kd position gain values: ');
            fprintf(mySerial,'%d %d %d\n',pgains);
            
            fprintf('Sending Kp = %d, Ki = %d and Kd = %d to the position controller.   ',pgains(1),pgains(2),pgains(3));
            
        case 'j'
            pgains1 = fscanf(mySerial, '%d %d %d');
            fprintf('The current controller is using Kp = %d, Ki = %d and Kd = %d.   ',pgains1(1),pgains1(2),pgains(3));
        case 'k'
            read_plot_matrix(mySerial);
        case 'l'
            ang = input('Enter the desired motor angle in degrees:  ');
            fprintf(mySerial,'%d\n',ang);
            
            fprintf('Moving to %d degrees.   ',ang);
        case 'm'
            steptraj = input('Enter step trajectory, in sec and degrees [time1,ang1;time2,ang2;...]: ');
           
            for i = 1:length(steptraj)
                if steptraj(i,1) > 10
                    fprintf('ERROR: maximum trajectory time is 10 seconds\n');
                end
            end
           
            ref = genRef(steptraj,'step');
            
            fprintf(mySerial,'%d\n',length(ref));
            
            for i = 1:length(ref)
                fprintf(mySerial,'%d\n',ref(1,i));
                if i == length(ref)
                    fprintf('Plotting the desired trajectory and sending to the PIC32 ... completed    ');
                end
            end
        case 'n'
            cubetraj = input('Enter cubic trajectory, in sec and degrees [time1,ang1;time2,ang2;...]: ');
            
            for i = 1:length(cubetraj)
                if cubetraj(i,1) > 10
                    fprintf('ERROR: maximum trajectory time is 10 seconds\n');
                end
            end
           
            ref = genRef(cubetraj,'cubic');
            
            fprintf(mySerial,'%d\n',length(ref));
            
            for i = 1:length(ref)
                fprintf(mySerial,'%2.2f\n',ref(1,i));
                if i == length(ref)
                    fprintf('Plotting the desired trajectory and sending to the PIC32 ... completed    ');
                end
            end
        case 'o'
            read_plot_matrix(mySerial);
        case 'p'
            fprintf('The motor is unpowered.   ');
        case 'q'
            has_quit = true;
        case 'r'
            mode = fscanf(mySerial, '%d');
            fprintf('The mode is %s   ', M(mode));
        otherwise
            fprintf('Invalid Selection %c\n   ',selection);
            
    end
end
end
