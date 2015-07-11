clear;
warning('off','all');
min_order = 2;
max_order = 200;
granularity = 0.01;
min_freq = 0.01;
max_freq = 0.99;

for order = min_order : max_order
    disp(['order ' num2str(order)]);
    for freq = min_freq : granularity : max_freq
        file = fopen(['filter_order' num2str(order) '_freq' num2str(freq) '.dat'],'w');
        try
        lp = fdesign.lowpass('N,Fc', order, freq);
        fir = design(lp,'window','window',@blackman,'SystemObject',true);
        fwrite(file, fir.Numerator, 'double', 0, 'l');
        catch
            disp(['error at order ' num2str(order) ' and freq ' num2str(freq)]);
        end
        fclose(file);
            
    end 
    
end