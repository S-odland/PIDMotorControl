function data = read_plot_matrix(mySerial)
nsamples = fscanf(mySerial, '%d')
data = zeros(nsamples,2);
for i = 1:nsamples
    data(i,:) = fscanf(mySerial, '%d %d');
    times(i) = (i-1)*0.2;
end

if nsamples>1
    stairs(times,data(:,1:2));
else
    fprintf('Only 1 sample recieve\n');
    disp(data);
end
score = mean(abs(data(:,1)-data(:,2)));
fprintf('\nAverage error: %5.1f mA\n',score);
title(sprintf('Average error: %5.1f mA',score));
ylabel('Current (mA)');
xlabel('Time (ms)');
end