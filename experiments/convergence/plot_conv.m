
AS = csvread('astro-ph_conv.csv');
CN = csvread('cnr-2000_conv.csv');
DE = csvread('delaunay_n18_conv.csv');
RG = csvread('rgg_2_18_conv.csv');

%% normalizing each data set 
AS(:,2) = 100*(AS(:,2)/AS(1,2));
CN(:,2) = 100*(CN(:,2)/CN(1,2));
DE(:,2) = 100*(DE(:,2)/DE(1,2));
RG(:,2) = 100*(RG(:,2)/RG(1,2));


Max_iter = 500;
ft_freq = 10;

DT = [AS(:,2),CN(:,2),DE(:,2),RG(:,2) ] ;       


[m,n] =size(DT);

linestyles = cellstr(char('-','-','-','-','-','-.','--','-',':','-',':',...
'-.','--','-',':','-.','--','-',':','-.'));

MarkerEdgeColors=hsv(n+1);  % n is the number of different items you have
Markers=['o','s','d','v','^','<','>','p','h','.',...
'+','*','o','x','*','x','^','<','h','+','.','>','p','s','d','v',...
'o','x','+','*','s','d','v','^','<','>','p','h','.'];

figure;
for i = 1:n
	
	plot(AS(:,1),DT(:,i),[linestyles{i} Markers(i)],'Color',MarkerEdgeColors(i,:),'LineWidth',1.5);
	hold on
end


set(gca,'XLim',[-10, -1]);
% set(gca,'YLim',[-10, 400]);
set(gca,'YLim',[0, 450]);

hTitle  = title ('Convergence Rate Vs. Fault Rate');
hXLabel = xlabel('Fault Rate f'                     );
hYLabel = ylabel('Normalized Increase in Iterations Ratio (%)');    

legend('astro-ph','cnr-2000','delaunay\_n18','rgg\_2\_18');
set(gcf, 'Color', 'w'); %setting the back ground color

set(gca, 'XTickLabel',[])                      %# suppress current x-labels

xt = get(gca, 'XTick');
yl = get(gca, 'YLim');
str = cellstr( num2str(xt(:),'2^{%d}') );      %# format x-ticks as 2^{xx}
hTxt = text(xt, yl(ones(size(xt))), str, ...   %# create text at same locations
    'Interpreter','tex', ...                   %# specify tex interpreter
    'VerticalAlignment','top', ...             %# v-align to be underneath
    'HorizontalAlignment','center');           %# h-aligh to be centered


% set(0,'DefaultAxesColorOrder',[1 0 0;0 1 0;0 0 1;0.5 0.5 0;0.5 0 0.5; 0 0.5 0.5;0.5 0.5 0.25;0.5 0.25 0.5; 0.25 0.5 0.5;],...
%       'DefaultAxesLineStyleOrder','-|--|:');
% semilogy(Mres,'LineWidth',1.00);

set(gca, ...
  'Box'         , 'off'     , ...
  'TickDir'     , 'out'     , ...
  'TickLength'  , [.02 .02] , ...
  'XMinorTick'  , 'off'      , ...
  'YMinorTick'  , 'off'      , ...
  'YGrid'       , 'on'      , ...
  'XColor'      , [.3 .3 .3], ...
  'YColor'      , [.3 .3 .3], ...
  'LineWidth'   , 1         );

% print2eps('plot_conv.eps'); %finally printing the files

