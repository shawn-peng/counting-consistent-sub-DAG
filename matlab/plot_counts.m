function [] = plot_counts(pfile, pttl, ffile, ufile, clr)
    %PLOT_COUNTS
    %
    %   [] = PLOT_COUNTS(pfile, pttl, ffile, ufile, clr);
    %
    %       Plots the counts of consistent subgraphs.
    %
    % Remark
    % ------
    %
    % Input
    % -----
    % [char]
    % pfile:    The filename of the plot.
    %           Note that the file extension must be one of 'eps' or 'png'.
    %           default: 'png'
    %
    % [char]
    % pttl:     The plot title.
    %
    % [char]
    % ffile:    The data file (4-column) of full ontologies
    %           <level> <count> <lower bound> <upper bound>
    %           Unknown values should be marked as 0.
    %
    % [char]
    % ufile:    The data file of used ontologies.
    %
    % [double]
    % clr:      1-by-3 color tuple.
    %
    % Output
    % ------
    % None.
    %
    % Dependency
    % ----------
    % [>] embed_canvas.m

    % check inputs {{{
    if nargin ~= 5
        error('plot_counts:InputCount', 'Expected 5 inputs.');
    end

    % pfile
    validateattributes(pfile, {'char'}, {'nonempty'}, '', 'pfile', 1);
    [p, f, e] = fileparts(pfile);
    if isempty(e)
        e = '.png';
    end
    ext = validatestring(e, {'.eps', '.png'}, '', 'pfile', 1);
    if strcmp(ext, '.eps')
        device_op = '-depsc';
    elseif strcmp(ext, '.png');
        device_op = '-dpng';
    end

    % pttle
    validateattributes(pttl, {'char'}, {'nonempty'}, '', 'pttl', 2);

    % ffile
    validateattributes(ffile, {'char'}, {'nonempty'}, '', 'ffile', 3);

    % ufile
    validateattributes(ufile, {'char'}, {'nonempty'}, '', 'ufile', 4);

    % clr
    validateattributes(clr, {'double'}, {'numel', 3}, '', 'clr', 5);
    % }}}

    % read data file (full) {{{
    data = textscan(fopen(ffile, 'r'), '%f%f%f%f');
    full_lvl = data{1};
    full_cnt = data{2};
    full_lbd = data{3};
    full_ubd = data{4};
    full_cnt(full_cnt == 0) = nan;
    % }}}

    % read data file (used) {{{
    data = textscan(fopen(ufile, 'r'), '%f%f%f%f');
    used_lvl = data{1};
    used_cnt = data{2};
    used_lbd = data{3};
    used_ubd = data{4};
    used_cnt(used_cnt == 0) = nan;
    % }}}

    % make plot {{{
    h = figure('Visible', 'off');
    hold on;
    bw = 0.8;

    fullcolor = clr;
    usedcolor = mean([[1 1 1]; clr]);
    % mixed     = [1 1 1] - ([1 1 1] - fullcolor) .* ([1 1 1] - usedcolor);
    mixed     = mean([[0 0 0]; clr]);

    % the full graph {{{
    for i = 1 : numel(full_lvl)
        height = max(0, full_ubd(i) - full_lbd(i));
        rectangle('Position', [full_lvl(i)-.5*bw, full_lbd(i), bw, height], 'FaceColor', fullcolor);
    end
    plot(full_lvl, full_cnt, 'k+-', 'LineWidth', 2);
    % }}}

    % the used graph {{{
    for i = 1 : numel(used_lvl)
        height = max(0, used_ubd(i) - used_lbd(i));
        
        rectangle('Position', [used_lvl(i)-.5*bw, used_lbd(i), bw, height], 'FaceColor', usedcolor);

        % special treatment for overlapping rectangles {{{
        if full_lbd(i) < used_ubd(i)
            height2 = used_ubd(i) - full_lbd(i);
            rectangle('Position', [used_lvl(i)-.5*bw, full_lbd(i), bw, height2], 'FaceColor', mixed);
        end
        % }}}
    end
    plot(used_lvl, used_cnt, '+:', 'Color', [.3 .3 .3], 'LineWidth', 2);
    % }}}

    % legend({'full', 'used'}, 'Location', 'se');

    ax               = gca;
    ax.XLim          = [1, 20];
    ax.Title.String  = pttl;
    ax.XLabel.String = 'levels';
    ax.YLabel.String = 'log_{10}(counts)';

    embed_canvas(h, 5, 2.5);
    print(h, pfile, device_op, '-r300');
    close;
    % }}}
end

% -------------
% Yuxiang Jiang (yuxjiang@indiana.edu)
% Department of Computer Science
% Indiana University, Bloomington
% Last modified: Wed 08 Nov 2017 12:32:51 PM E
