function [] = plot_entropy(pfile, pttl, ratios, lb_ratios, ub_ratios)
    %PLOT_ENTROPY
    %
    %   [] = PLOT_ENTROPY(pfile, pttl, ratios, lb_ratios, ub_ratios);
    %
    %       Plots relative entropy: observed / maximum.
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
    % [cell]
    % ratios:   A 1-by-4 cell array of precomputed entropy ratios for the 4
    %           ontologies in the order of {mfo, bpo, cco, hpo}.
    %
    % Output
    % ------
    % None.
    %
    % Dependency
    % ----------
    % [>] pfp_rgby.m
    % [>] embed_canvas.m

    % check inputs {{{
    if nargin ~= 5
        error('plot_entropy:InputCount', 'Expected 5 inputs.');
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

    % ratios
    validateattributes(ratios, {'cell'}, {'numel', 4}, '', 'ratios', 3);

    % lb_ratios
    validateattributes(lb_ratios, {'cell'}, {'numel', 4}, '', 'lb_ratios', 4);

    % ub_ratios
    validateattributes(ub_ratios, {'cell'}, {'numel', 4}, '', 'ub_ratios', 5);
    % }}}

    % plot {{{
    h = figure('Visible', 'off');
    hold on;

    c = cell(1, 4);
    [r g b y] = pfp_rgby;
    c{1} = b;
    c{2} = r;
    c{3} = y;
    c{4} = g;

    % for i = 1 : 4
    %     levels = 1:numel(ratios{i});
    %     x = [levels, fliplr(levels)];
    %     y = [lb_ratios{i}', fliplr(ub_ratios{i}')];
    %     patch(x, y, c{i}, 'FaceColor', 'flat', 'EdgeColor', c{i}, 'LineStyle', '-.');
    % end

    for i = 1 : 4
        levels = 1:numel(ratios{i});
        mean_ratios = mean([lb_ratios{i} ub_ratios{i}]')';
        plot(levels, mean_ratios, ':', 'Color', c{i}, 'LineWidth', 1.2);
        plot(levels, ratios{i}, 'o-', 'Color', c{i}, 'LineWidth', 2);
        for j = 1 : numel(levels)
            if isnan(ratios{i}(j))
                loc_errbar(gca, levels(j), mean_ratios(j), lb_ratios{i}(j), ub_ratios{i}(j), c{i});
            end
        end
    end

    % legend {{{
    % legend({'MFO', 'BPO', 'CCO', 'HPO'}, 'Location', 'NE');
    legend_anchor = [18 .5];
    loc_tag(gca, legend_anchor, c{1}, 'MFO');
    legend_anchor(2) = legend_anchor(2) / 2;
    loc_tag(gca, legend_anchor, c{2}, 'BPO');
    legend_anchor(2) = legend_anchor(2) / 2;
    loc_tag(gca, legend_anchor, c{3}, 'CCO');
    legend_anchor(2) = legend_anchor(2) / 2;
    loc_tag(gca, legend_anchor, c{4}, 'HPO');
    % }}}

    ax               = gca;
    ax.YScale        = 'log';
    ax.Title.String  = pttl;
    ax.XLabel.String = 'level';
    ax.XLim          = [2, 20];

    embed_canvas(h, 8, 3);
    print(h, pfile, device_op, '-r300');
    close;
    % }}}
end


% function loc_errbar {{{
function [] = loc_errbar(ax, x, y, lb, ub, c)
    line(ax, [x, x], [lb, ub], 'LineWidth', 1.5, 'Color', c);
    line(ax, [x-.1, x+.1], [lb lb], 'LineWidth', 1.5, 'Color', c);
    line(ax, [x-.1, x+.1], [ub ub], 'LineWidth', 1.5, 'Color', c);
end
% }}}

% function loc_tag {{{
function [] = loc_tag(ax, ctr, c, str)
    len = 0.5;
    line(ax, [ctr(1)-len ctr(1)], [ctr(2) ctr(2)], 'LineStyle', '-', 'LineWidth', 2, 'Color', c);
    line(ax, [ctr(1) ctr(1)+len], [ctr(2) ctr(2)], 'LineStyle', ':', 'LineWidth', 1.2, 'Color', c);

    % % make a circle {{{
    % theta = 0 : 0.01 : 2*pi;
    % radius = 0.1;
    % x = radius * cos(theta);
    % y = radius * sin(theta);
    % xe = exp(x);
    % ye = exp(y);
    % semilogy(x+ctr(1), ye+ctr(2), 'LineWidth', 2, 'Color', c);
    % % }}}

    text(ctr(1)+len+0.1, ctr(2), str);
end
% }}}

% -------------
% Yuxiang Jiang (yuxjiang@indiana.edu)
% Department of Computer Science
% Indiana University, Bloomington
% Last modified: Tue 31 Oct 2017 05:15:02 PM E
