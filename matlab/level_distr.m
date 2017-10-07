function [] = level_distr(pfile, ont, l)
    %LEVEL_DISTR
    %
    %   [] = LEVEL_DISTR(pfile, ont, l);
    %
    %       Bar plots of terms by levels.
    %
    % Input
    % -----
    % [char]
    % pfile:    The output file name.
    %
    % [struct]
    % ont:      The ontology structure, see pfp_ontbuild.m
    %
    % [double]
    % l:        The maximum ontology to highlight.
    %
    % Output
    % ------
    % None.
    %
    % Dependency
    % ----------
    % [>] pfp_level.m
    % [>] pfp_rgby.m
    % [>] embed_canvas.m

    % check inputs {{{
    if nargin ~= 3
        error('level_distr:InputCount', 'Expected 3 inputs.');
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

    % l
    validateattributes(l, {'double'}, {'positive', 'integer'}, '', 'l', 3);
    % }}}

    % collect data {{{
    levels = pfp_level(ont, ont.term);
    [r, g, b, y] = pfp_rgby;
    if strcmp(ont.ont_type, 'molecular_function')
        ontstr = 'MFO';
        hc = b;
    elseif strcmp(ont.ont_type, 'biological_process')
        ontstr = 'BPO';
        hc = r;
    elseif strcmp(ont.ont_type, 'cellular_component')
        ontstr = 'CCO';
        hc = y;
    elseif strcmp(ont.ont_type, 'human_phenotype')
        ontstr = 'HPO';
        hc = g;
    else
        error('level_distr:InputErr', 'Unknown ontology type');
    end
    grey = [0.5 0.5 0.5];
    bw = 0.9;
    % }}}
 
    % plot {{{
    h = figure('Visible', 'off');
    ax = gca;

    [L, ~, index] = unique(levels);
    for i = 1 : max(L)
        counts = sum(index == i);
        rpos = [i - bw/2, 0, bw, counts];
        if i <= l
            rectangle('Position', rpos, 'FaceColor', hc, 'EdgeColor', hc);
        else
            rectangle('Position', rpos, 'FaceColor', grey, 'EdgeColor', grey);
        end
    end

    ax.Title.String = ['Term levels of ', ontstr];
    ax.XLabel.String = 'levels';
    ax.YLabel.String = 'counts';
    ax.XLim = [0 max(L)+1];

    embed_canvas(h, 5, 2);
    print(h, pfile, device_op, '-r300');
    close;
    % }}}
end

% -------------
% Yuxiang Jiang (yuxjiang@indiana.edu)
% Department of Computer Science
% Indiana University, Bloomington
% Last modified: Sun 23 Jul 2017 05:52:03 AM E
