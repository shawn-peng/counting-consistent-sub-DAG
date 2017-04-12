function [] = plot_degree_distribution(pfile, pttl, ont, etype)
    %PLOT_DEGREE_DISTRIBUTION
    %
    %   [h] = PLOT_DEGREE_DISTRIBUTION(pfile, pttl, ont, etype);
    %
    %       Plots the in-degree distribution of an ontology.
    %
    % Remark
    % ------
    % This function assumes top-down direction of all edges with the root on the
    % top, i.e., "general terms" -> "specific terms".
    %
    % Input
    % -----
    % [char]
    % pfile:    The filename of the plot.
    %           Note that the file extension should be either 'eps' or 'png'.
    %           default: 'png'
    %
    % [char]
    % pttl:     The plot title.
    %
    % [struct]
    % ont:      The ontology structure, see pfp_ontbuild.m
    %
    % [char]
    % etype:    Edge type, available options are:
    %           'any'   in-/out-degree
    %           'in'    in-degree
    %           'out'   out-degree
    %           default: 'any'.
    %
    % Output
    % ------
    % None.
    %
    % Dependency
    % ----------
    % [>] embed_canvas.m;

    % check inputs {{{
    if nargin ~= 3 && nargin ~= 4
        error('plot_degree_distribution:InputCount', 'Expected 3 or 4 inputs.');
    end

    if nargin == 3
        etype = 'any';
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

    % pttl
    validateattributes(pttl, {'char'}, {}, '', 'pttl', 2);

    % ont
    validateattributes(ont, {'struct'}, {'nonempty'}, '', 'ont', 3);

    % etype
    etype = validatestring(etype, {'any', 'in', 'out'}, '', 'etype', 4);
    % }}}

    % collect data and plot {{{
    switch etype
        case 'any'
            counts = reshape(full(sum(ont.DAG, 1)), 1, []) + reshape(full(sum(ont.DAG, 1)), 1, []);
            xlabelstring = 'degree';
        case 'in'
            counts = reshape(full(sum(ont.DAG, 2)), 1, []);
            xlabelstring = 'in-degree';
        case 'out'
            counts = reshape(full(sum(ont.DAG, 1)), 1, []);
            xlabelstring = 'out-degree';
        otherwise
            error('plot_degree_distribution:ETypeERR', 'Unknown edge type.');
    end

    h = figure('Visible', 'off');
    hold on;
    histogram(counts);
    ax = gca;
    ax.Title.String = pttl;
    ax.XLabel.String = xlabelstring;
    ax.YLabel.String = 'fraction';
    ax.XLim(1) = 0;
    ax.YScale = 'log';
    % ax.XScale = 'log';

    embed_canvas(h, 5, 4); % 5 x 4 inches
    print(h, pfile, device_op, '-r300');
    close;
    % }}}
end
