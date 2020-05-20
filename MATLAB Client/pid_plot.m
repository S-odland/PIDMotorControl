function data = pid_plot(port,Kp,Ki)

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

fprintf('opening port %s....\n',port);

mySerial = serial(port, 'BaudRate',230400, 'FlowControl','hardware');
fopen(mySerial);
clean = onCleanup(@()fclose(mySerial));

fprintf('Setting Kp = %f, Ki = %f\n', Kp, Ki);
fprintf(mySerial,'%f %f\n',[Kp,Ki]);
fprintf('Waiting for samples ...\n');

sampnum = 1;
read_samples = 10;

while read_samples>1
    data_read = fscanf(mySerial, '%d %d %d');
    
    read_samples = data_read(1);
    ADCval(sampnum) = data_read(2);
    ref(sampnum) = data_read(3);
    
    sampnum = sampnum + 1;
end
data = [ref;ADCval];

clf;
hold on;
t = 1:sampnum-1;
plot(t,ref);
plot(t,ADCval);
legend('Reference', 'ADC Value');
title(['Kp: ',num2str(Kp),' Ki: ',num2str(Ki)]);
ylabel('Brightness (ADC counts)');
xlabel('Sample Number (at 100 Hz)');
hold off;
end