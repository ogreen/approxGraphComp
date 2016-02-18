%% read teh data 
AS = csvread('Astro-ph-overhead-iteration.csv');


% draw the figure 
figure;

bar(100*AS,'stacked','EdgeColor',[.2 .2 .2],'LineWidth',1.02);

%%
hTitle  = title ('Fault Tolerant Algorithm Overhead at f=2^{-6}|E|');
hXLabel = xlabel('Iteration'                     );
hYLabel = ylabel('Normalized Time (%)');    


% set(gca,'XLim',[0, 16]);
% set(gca,'YLim',[-10, 400]);
set(gca,'YLim',[0, 50]);

set(gcf, 'Color', 'w'); %setting the back ground color

% set(gca, 'XTickLabel',GraphNames);

% xticklabel_rotate([],45);


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

legend('Baseline' ,'Auxiliary Data Structure+Fault Detection Overhead','Fault Correction Overhead') 
colormap hsv(4);