function [] = plot_by_levels(pfile, ont, fc, uc, lb)
    %PLOT_BY_LEVELS
    %
    %   [] = PLOT_BY_LEVELS(pfile, ont, fc, uc);
    %   [] = PLOT_BY_LEVELS(pfile, ont, fc, uc, lb);
    %
    %       Plots the cdag counts of an ontology level by level.
    %
    % Input
    % -----
    % (required)
    % [char]
    % pfile:    The output filename.
    %           Note that the file extension should be either 'eps' or 'png'.
    %           default: 'png'
    %
    % [char]
    % ont:      The ontology structure, see pfp_ontbuild.m
    %
    % [char]
    % fc:       File of full ontology counts by level.
    %
    % [char]
    % uc:       File of used ontology counts by level.
    %
    % (optional)
    % [char]
    % lb:       File of lower bounds by level.
    %
    % Output
    % ------
    % None.
    %
    % Dependency
    % ----------
    % [>] pfp_rgby.m
    % [>] pfp_level.m
    % [>] embed_canvas.m

    % check inputs {{{
    if nargin < 4 || nargin > 5
        error('plot_by_levels:InputCount', 'Expected 4 or 5 inputs.');
    end

    if nargin == 4
        lb = '';
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
    elseif strcmp(ext, '.png')
        device_op = '-dpng';
    end

    % ont
    validateattributes(ont, {'struct'}, {'nonempty'}, '', 'ont', 2);

    % fc
    validateattributes(fc, {'char'}, {'nonempty'}, '', 'fc', 3);

    % uc
    validateattributes(uc, {'char'}, {'nonempty'}, '', 'uc', 4);

    % lb
    validateattributes(lb, {'char'}, {}, '', 'lb', 5);
    % }}}

    % collect data {{{
    fcdata = load(fc);
    ucdata = load(uc);
    L = pfp_level(ont);
    [r, g, b, y] = pfp_rgby;
    if strcmp(ont.ont_type, 'molecular_function')
        ontstr = 'MFO';
        fcolor = b;
    elseif strcmp(ont.ont_type, 'biological_process')
        ontstr = 'BPO';
        fcolor = r;
    elseif strcmp(ont.ont_type, 'cellular_component')
        ontstr = 'CCO';
        fcolor = y;
    elseif strcmp(ont.ont_type, 'human_phenotype')
        ontstr = 'HPO';
        fcolor = g;
    else
        error('level_distr:InputErr', 'Unknown ontology type');
    end
    ucolor = (ones(1, 3) + fcolor) / 2;

    if ~isempty(lb)
        lbdata = load(lb);
    end
    % }}}

    % plot {{{
    h = figure('Visible', 'off');
    hold on;
    ax = gca;
    bw = 0.9;

    for i = 1 : size(fcdata, 1)
        x = fcdata(i, 1);
        rpos = [x - bw/2, 0, bw/2-0.01, fcdata(i, 2)];
        rectangle(ax, 'Position', rpos, 'FaceColor', fcolor, 'EdgeColor', fcolor);
    end

    for i = 1 : size(ucdata, 1)
        x = ucdata(i, 1);
        rpos = [x+0.01, 0, bw/2-0.01, ucdata(i, 2)];
        rectangle(ax, 'Position', rpos, 'FaceColor', ucolor, 'EdgeColor', ucolor);
    end

    if ~isempty(lb)
        lbdata(:,1) = lbdata(:,1) - bw/4;
        plot(lbdata(:,1), lbdata(:,2), 'k+:');
    end

    ax.Title.String = ['Number of consistent subgraphs in ', ontstr];
    ax.XLabel.String = 'levels';
    ax.YLabel.String = 'log(counts)';

    % ax.XLim = [0 20];
    % ax.YLim = [0 3000];

    ax.XLim = [0 8];
    ax.YLim = [0 600];

    % legend
    % axw = ax.XLim(2) - ax.XLim(1);
    % axh = ax.YLim(2) - ax.YLim(1);
    % lposf = [0.65, 0.15, 0.1, 0.03] .* [axw, axh, axw, axh];
    % lposu = [0.65, 0.08, 0.1, 0.03] .* [axw, axh, axw, axh];

    % rectangle(ax, 'Position', lposf, 'FaceColor', fcolor, 'EdgeColor', fcolor);
    % rectangle(ax, 'Position', lposu, 'FaceColor', ucolor, 'EdgeColor', ucolor);
    % text(ax, 0.78*axw, 0.165*axh, 'all terms', 'VerticalAlignment', 'middle');
    % text(ax, 0.78*axw, 0.095*axh, 'used terms', 'VerticalAlignment', 'middle');

    embed_canvas(h, 5, 3);
    print(h, pfile, device_op, '-r300');
    close;
    % }}}
end

% -------------
% Yuxiang Jiang (yuxjiang@indiana.edu)
% Department of Computer Science
% Indiana University, Bloomington
% Last modified: Thu 20 Jul 2017 10:58:23 PM E
